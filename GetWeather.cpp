//g++ -o GetWeather GetWeather.cpp -lcurl -ljsoncpp
#include "GetWeather.h"
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>
#include <string>

struct url_data {
	size_t size;
	char *data;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data){
	size_t index = data->size;
	size_t n = (size * nmemb);
	void *tmp;

	data->size +=(size*nmemb);
	tmp = realloc((void *)data->data, data->size+1);

	if(tmp){
		data->data = (char *)tmp;
	}else {
		free(data->data);
		fprintf(stderr, "Failed to allocate memory.h");
		return 0;
	}

	memcpy((data->data+index), ptr, n);
	data->data[data->size] = '\0';

	return size * nmemb;
}

string getWeather(const string &city){
	CURL *curl;
	CURLcode res;
	struct url_data data;
	data.size = 0;
	data.data = (char *)malloc(4096);
	if(NULL == data.data){
		fprintf(stderr, "Failed to allocate memory\n");
		return NULL;
	}

	data.data[0] = '\0';

	curl = curl_easy_init();
	if(curl){
		struct curl_slist *chunk = NULL;
		chunk = curl_slist_append(chunk, "apikey:e6c194f817d5ce9880eedc2a7eb694cc");
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
		curl_easy_setopt(curl, CURLOPT_URL, "http://apis.baidu.com/apistore/weatherservice/cityname?cityname=北京");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

		res = curl_easy_perform(curl);
		
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		curl_easy_cleanup(curl);
		curl_slist_free_all(chunk);

		return data.data;
	}

}

string parseJson(string strJson){
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(strJson.c_str(), root);
	if(!parsingSuccessful){
	//	std::cout<<"Failed to parse"<<reader.getFormattedErrorMessages();
		return 0;
	}
	string requestRes =  root.get("errMsg","A Default Value").asString();
	if(requestRes == "success"){
		Json::Value detail = root.get("retData","failed");
		return detail.get("weather","unknown").asString();
	}

	return "failed";
}

int main(){
	cout<<parseJson(getWeather("深圳"))<<endl;
	return 0;
}
