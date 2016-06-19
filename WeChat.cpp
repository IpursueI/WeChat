#include <vector>  
#include <string>  
#include <stdio.h>  
#include <stdlib.h> 
#include <cstring>
#include <map>
#include <regex>
#include <tinyxml.h>
#include <ctime>

#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>  

#include "CheckSignature.h"
#include "GetWeather.h"

using namespace std;
using namespace cgicc;

//构造返回的数据格式
string createTextReplyData(const string &msgType, const string &toUserName, const string &fromUserName, const string &content){
	
	
	string data = "<xml><ToUserName><![CDATA["+fromUserName+"]]></ToUserName><FromUserName><![CDATA["+toUserName
		+"]]></FromUserName><CreateTime>"+to_string(time(NULL))+"</CreateTime><MsgType><![CDATA["+"text"
		+"]]></MsgType><Content><![CDATA["+content+"]]></Content></xml>";

	return data;
}

//对post过来的文本消息进行处理
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

	size_t spaceIdx = content.find(" ");
	if(spaceIdx != string::npos){
		string function = content.substr(0, spaceIdx+1);
		string funcContent = content.substr(spaceIdx+1, content.size()-spaceIdx-1);
		
		if(function == "天气 "){
			cout<<"Content-type:text/text/html\r\n\r\n";
			cout<<createTextReplyData(msgType, toUserName, fromUserName, getWeather(funcContent));
		}else{
			cout<<"Content-type:text/text/html\r\n\r\n";
			cout<<createTextReplyData(msgType, toUserName, fromUserName, "不支持该功能");
		}
	}else{
		cout<<"Content-type:text/text/html\r\n\r\n";
		cout<<createTextReplyData(msgType, toUserName, fromUserName, "输入格式有误");
	}
}


void processEvent(std::map<string, string> &data){

	string msgType, toUserName, fromUserName, event;
	
	msgType = data["MsgType"];

	if(data.find("ToUserName") != data.end()){
		toUserName = data["ToUserName"];
	}
	if(data.find("FromUserName") != data.end()){
		fromUserName = data["FromUserName"];
	}
	if(data.find("Event") != data.end()){
		event = data["Event"];
	}


	if(event == "subscribe"){
		cout<<"Content-type:text/text/html\r\n\r\n";
		string content = "感谢订阅公众号，目前还处在开发测试阶段。有新功能添加后会第一时间通知您，敬请期待。";
		cout<<createTextReplyData(msgType, toUserName, fromUserName, content);
	}
}
//对post过来的xml消息进行解析，并存储到字典当中
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
		}else if(resData["MsgType"] == "event"){ 
			cout<<"Content-type:text/text/html\r\n\r\n";
			cout<<createTextReplyData(resData["MsgType"], resData["ToUserName"], resData["FromUserName"], "为何不能处理event类型消息");
			//processEvent(resData);
		}else {
			cout<<"Content-type:text/text/html\r\n\r\n";
			cout<<createTextReplyData(resData["MsgType"], resData["ToUserName"], resData["FromUserName"], "暂不支持该消息类型");
		}
	}

}

//服务器验证
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

//处理post消息
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
