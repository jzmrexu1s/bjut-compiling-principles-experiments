#include <iostream>
#include <map>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <string.h>
using namespace std;

struct state{
	vector<int> lstr;					//����ʽ�� 
	vector<vector<int> > rstr; 			//����ʽ�Ҳ� 
	vector<int> idx;					//��Ӧ.���±� 
} G0;

map<string,int> token0;
map<string,bool> terminal; 
map<int,string> itoken0;

vector<string> V,T;
vector<string> target_str;

void read_token(string file){
	ifstream infile; 
    infile.open(file.data());   //���ļ����������ļ��������� 
    assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 
    string s;
    int cnt=0,flag=0;
    //����token�� 
    token0["Start"]=0;terminal["Start"]=0;
    itoken0[0] = "Start";
    V.push_back("Start");
    while(getline(infile,s))
    {
    	if(s=="terminal:"){
    		flag=1;continue;
		}
		if(flag) T.push_back(s);
		else V.push_back(s);
		terminal[s]=flag;
    	token0[s]=++cnt;
    	itoken0[cnt] = s;
    }
    infile.close();             //�ر��ļ������� 
}

void read_production(string file)
{	
    ifstream infile; 
    infile.open(file.data());   //���ļ����������ļ��������� 
    assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 
    string s;
    int flag=0;
    while(getline(infile,s))
    {
    	int pos=s.find("->");
    	string ltmp=s.substr(0,pos);
		string rtmp=s.substr(pos+2);
		string tmpstr="";
		vector<int> vec;
		//����token��ʶ�� 
		for(int i=0;i<rtmp.length();i++){
			if(rtmp[i]==' ') continue;
			tmpstr+=rtmp[i];
			if(token0.count(tmpstr)&&(i==rtmp.length()-1||rtmp[i+1]==' ')){
				vec.push_back(token0[tmpstr]);
				tmpstr="";
			}
		}
		if(!flag){
			vector<int> fvec;
			fvec.push_back(token0[ltmp]);
			G0.lstr.push_back(0);G0.rstr.push_back(fvec);G0.idx.push_back(-1);
			G0.lstr.push_back(0);G0.rstr.push_back(fvec);G0.idx.push_back(0);
			flag=1;
		}
		//��������ʽ�����ع��ķ� 
		for(int i=0;i<=vec.size();i++){
			G0.lstr.push_back(token0[ltmp]);
			G0.rstr.push_back(vec);
			G0.idx.push_back(i-1);
		}
    }
    infile.close();             //�ر��ļ������� 
}

void read_string(string file){
	ifstream infile; 
    infile.open(file.data());   //���ļ����������ļ��������� 
    assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 
    string s;
    while(getline(infile,s)){
    	target_str.push_back(s);
	}
    infile.close();             //�ر��ļ������� 
}


//state closure(state I){
//	state J = I;
//	int flag = 0;
//	while(true){
//		for(int i = 0; i < J.rstr.size(); i++){
//			; 
//		}
//	}
//} 

void pre1(){
	read_token("token.txt");
	read_production("production.txt");
	read_string("string.txt");
}
//
//int main(){
//	read_token("token.txt");
//	read_production("production.txt");
//	for(int i=0;i<G.idx.size();i++){
//		cout<<G.lstr[i]<<"   ";
//		for(int j=0;j<G.rstr[i].size();j++) cout<<G.rstr[i][j]<<" ";
//		cout<<"     "<<G.idx[i]<<endl;
//	}
//	return 0;
//}
