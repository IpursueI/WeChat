#include "CheckSignature.h"

bool checkSignature(const string &signature, const string &timestamp, const string &nonce) {

	string token = "wechat";
	vector<string> tmpV;

	tmpV.push_back(token);
	tmpV.push_back(timestamp);
	tmpV.push_back(nonce);

	sort(tmpV.begin(), tmpV.end());

	string origin = tmpV[0]+tmpV[1]+tmpV[2];

	unsigned char digest[20] = {0};
	SHA1((const unsigned char *)origin.c_str(), origin.size(), digest);
	
    string strResult;  
    char sHexStr[32];  
	for(int i = 0; i < sizeof(digest);i++ ){  
		snprintf( sHexStr, sizeof(sHexStr),"%x%x", ((int)digest[i] & 0xf0) >> 4, ((int)digest[i] &0x0f) );  
		strResult.append(sHexStr);  
	}

	if(signature == strResult){
		return true;
	}else {
		return false;
	}
}
