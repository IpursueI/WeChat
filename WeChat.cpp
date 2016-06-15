#include <vector>  
#include <string>  
#include <stdio.h>  
#include <stdlib.h> 
#include <cstring>
#include <map>
#include <regex>
#include <tinyxml.h>

#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>  

#include "CheckSignature.h"

using namespace std;
using namespace cgicc;

string createTextReplyData(string &msgType, string &toUserName, string &fromUserName, string &content){
	
	
	string data = "<xml><ToUserName><![CDATA["+fromUserName+"]]></ToUserName><FromUserName><![CDATA["+toUserName
		+"]]></FromUserName><CreateTime>123456</CreateTime><MsgType><![CDATA["+"text"
		+"]]></MsgType><Content><![CDATA["+content+"]]></Content></xml>";

	return data;
}

void processText(std::map<string, string> &data){

	string msgType, toUserName, fromUserName, content;
	
	msgType = data["MsgType"];

	if(data.find("ToUserName") != data.end()){
		toUserName = data["ToUserName"];
	}
	if(data.find("FromUserName") != data.end()){
		fromUserName = data["FromUserName"];
	}
	if(data.find("Content") != data.end()){
		content = data["Content"];
	}

	cout<<"Content-type:text/text/html\r\n\r\n";
	cout<<createTextReplyData(msgType, toUserName, fromUserName, content);
}

void parseRawData(const string& data){

	//cgicc中也包含map，不加std会冲突
	std::map<string, string> resData;


	TiXmlDocument doc;
	doc.Parse(data.c_str(), 0, TIXML_ENCODING_UTF8);
	TiXmlElement *rootElement = doc.RootElement();
	TiXmlElement *element = rootElement->FirstChildElement();

	while(element){
		resData[element->ValueStr()] = element->GetText();
		element = element->NextSiblingElement();
	}
	
	if(resData.find("MsgType") != resData.end()){
		if(resData["MsgType"] == "text"){
			processText(resData);
		}
	}

}

void processGet(Cgicc &formData) {
	
	cout<<"Content-type:text/text/html\r\n\r\n";
	string signature,timestamp,nonce,echostr;

	form_iterator fi = formData.getElement("signature");

	if(fi != formData.getElements().end()){
		signature = **fi;
	}

	fi = formData.getElement("timestamp");

	if(fi != formData.getElements().end()){
		timestamp = **fi;
	}

	fi = formData.getElement("nonce");

	if(fi != formData.getElements().end()){
		nonce = **fi;
	}

	fi = formData.getElement("echostr");

	if(fi != formData.getElements().end()){
		echostr = **fi;
	}

	if(checkSignature(signature, timestamp, nonce)){
		cout<<echostr;
	}else{
		cout<<"";
	}
}


void processPost(const Cgicc &formData){
	
	//通过CgiEnvironment获取所有的PostData
	const CgiEnvironment cgiEnv = formData.getEnvironment();
	string body = cgiEnv.getPostData();

	parseRawData(body);
}

int main() {

	Cgicc formData;
	char *req_method;

	req_method = getenv("REQUEST_METHOD");
	
	if(!strcmp(req_method, "GET")){
		processGet(formData);
	}else if(!strcmp(req_method, "POST")){
		processPost(formData);
	}


	return 0;
}
//sudo g++ -std=c++11 -o WeChat.cgi WeChat.cpp CheckSignature.cpp -lcgicc -lssl -lcrypto -ltinyxml
