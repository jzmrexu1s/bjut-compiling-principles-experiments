#include <iostream>
#include <map>
#include <stack> 
#include <queue>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <string.h>
#include "pre.h"
#define acc 999 
using namespace std;
state G;													//�ع��ķ� 
vector<state> C;											//��Ŀ���淶�� 
vector<int> target;											//������������ɲ��� int�� 
vector<string> target_component;							//������������ɲ��� string�� 

map<string,int> token;										//����tokenֱ�Ӳ��������±� 
map<int,string> itoken;										//����token���±�����ӳ����ַ�����ʽ 

stack<int> ssta;											//״̬ջ 
stack<int> xsta;											//����ջ�����ڴ�Ż�ǰ׺ 
stack<string> strsta;										//���ڴ����Ҫ������ս�� 

int action[100][100],go_to[100][100];						//action��go_to�� 
int ip;														//ָ����һ������������ 

void printstate(state res);									//��ӡһ��state������ 
void print_ans_table();										//��ӡ������ 
bool judge(vector<int> &j_str, state j_state);				//�жϲ���ʽ�Ƿ��Ѿ���state�У������λ��Ϊ-1�� 
bool judge_equal(vector<int> &vec1,vector<int> &vec2);		//�ж�����vector�Ƿ���ȣ������жϲ���ʽ�Ҳ��Ƿ���� 
bool judge_in(state j_state);								//�ж��µ�state�Ƿ��Ѿ��ڵ�ǰ��Ŀ���淶��C�� 
int judge_type(int j_lstr,vector<int> &j_rstr,int j_idx);	//�жϲ���ʽ�����ͣ�ר���ڷ������� 
int find_ans_idx(int f_lstr,vector<int> &f_rstr,int f_idx);	//���ع��ķ�G�л�ȡ����ʽ�±� 
int find_state_idx(state f_state);							//��ȡ״̬���±� 
bool need_output(string tt);								//�Ƿ�����Ҫ������ս����Ŀǰ������ʵ����ı�׼�ķ���Ч�� 
vector<int> fc(state f_state);								//�ж�һ��״̬�����ж��ٸ��ƽ�ѡ�����vector��ʽ���� 
state closure(state I);										//��һ��״̬�ıհ� 
state transfer_function(state I,int next_token);			//��ת�ƺ��� 
void proC();												//����Ŀ���淶��C 
void ans_table_create(); 									//���ɷ����� 
void compile_vision();										//����ʶ���������� 
void LR();													//�﷨�������� 


void printstate(state res){
	//���δ�ӡ״̬�е����� 
	for(int i=0;i<res.idx.size();i++){
		cout<<itoken[res.lstr[i]]<<"   ";
		if(res.idx[i]==-1) cout<<".";
		for(int j=0;j<res.rstr[i].size();j++){
			cout<<itoken[res.rstr[i][j]];
			if(j==res.idx[i]) cout<<".";
		} 
		cout<<"  "<<res.idx[i]<<endl;
	}
}

void print_ans_table(){
	int wid=6;
	cout.width(wid*T.size()+wid);
	cout<<"action";cout<<"|";
	cout.width(wid*V.size()-wid);
	cout<<"goto"<<endl;
	for(int i=0;i<T.size();i++) cout.width(wid),cout<<T[i];
	cout.width(wid);cout<<"#";cout<<"|";
	for(int i=1;i<V.size();i++) cout.width(wid),cout<<V[i];
	cout<<endl;
	for(int i=0;i<C.size();i++){
		for(int j=V.size();j<=token.size();j++){
			cout.width(wid);
			//999��ָ�������� 
			if(action[i][j]==999) cout<<"end";
			else cout<<action[i][j];
		} cout<<"|";
		for(int j=1;j<V.size();j++) cout.width(wid),cout<<go_to[i][j];
		cout<<endl;
	} 
}

bool judge(vector<int> &j_str, state j_state){
	//����J��ÿһ������ʽ�������Ĳ���ʽ�ȶ� 
	for(int i=0;i<j_state.rstr.size();i++){
		int flag=0;		//���ڱ���Ƿ���ͬ0��ͬ��1��ͬ 
		if(j_state.idx[i]!=-1) continue;	//�����ǰ����ʽ��λ�ò���-1����ֱ������ 
		for(int j=0;j<j_str.size();j++){	//��һ�ȶ� 
			if((j_str[j]!=j_state.rstr[i][j])||(j==j_state.rstr[i].size())){
				flag=1;break;
			}
		}
		//ȫ����ͬ�ҳ���Ҳ��ͬ 
		if(flag==0&&j_str.size()==j_state.rstr[i].size()) return true;	//���ֹ� 
	}
	return false;
}

bool judge_equal(vector<int> &vec1,vector<int> &vec2){
	if(vec1.size()!=vec2.size()) return false;
	else{
		for(int i=0;i<vec1.size();i++){
			if(vec1[i]!=vec2[i]) return false;
		}
	}
	return true;
}

bool judge_in(state j_state){
	for(int i=0;i<C.size();i++){
		if(j_state.idx.size()!=C[i].idx.size()) continue;
		//������״̬�е�ÿһ�� 
		int flag=0;
		for(int j=0;j<j_state.idx.size();j++){
			for(int k=0;k<C[i].idx.size();k++){
				if(judge_equal(j_state.rstr[j],C[i].rstr[k])&&j_state.lstr[j]==C[i].lstr[k]&&j_state.idx[j]==C[i].idx[k]){
					flag++;break;
				}
			}
			if(flag!=j+1) break;
		}
		if(flag==j_state.idx.size()) return true;
	}
	return false;
}

int judge_type(int j_lstr,vector<int> &j_rstr,int j_idx){
	//�������ʽ��Start->S. 
	if(j_lstr==0&&j_rstr[0]==1&&j_idx==0) return -2;
	//�������ʽ��A->alpha. 
	if(j_idx==j_rstr.size()-1) return -1;
	else{
		//������һ��token�ı�� 
		return j_rstr[j_idx+1];
	}
}

int find_ans_idx(int f_lstr,vector<int> &f_rstr,int f_idx){
	//����G�����еĲ���ʽ 
	for(int i=0;i<G.idx.size();i++){
		if(G.lstr[i]!=f_lstr) continue;
		if(!judge_equal(G.rstr[i],f_rstr)) continue;
		if(G.idx[i]!=f_idx) continue;
		return i;
	}
	return -1;
}

int find_state_idx(state j_state){
	for(int i=0;i<C.size();i++){
		if(j_state.idx.size()!=C[i].idx.size()) continue;
		//������״̬�е�ÿһ�� 
		int flag=0;
		for(int j=0;j<j_state.idx.size();j++){
			for(int k=0;k<C[i].idx.size();k++){
				if(judge_equal(j_state.rstr[j],C[i].rstr[k])&&j_state.lstr[j]==C[i].lstr[k]&&j_state.idx[j]==C[i].idx[k]){
					flag++;break;
				}
			}
			if(flag!=j+1) break;
		}
		if(flag==j_state.idx.size()) return i;
	}
	return -1;
}

bool need_output(string tt){
//	if(tstr.size()!=1) return false;
	if(tt=="IDN") return true;
	if(tt=="INT10") return true;
	if(tt=="INT16") return true; 
	if(tt=="INT8") return true; 
	return false;
}

vector<int> fc(state f_state){
	vector<int> fcres;
	int invec[100];
	memset(invec,0,sizeof(invec));
	//����ÿ������ʽ 
	for(int i=0;i<f_state.idx.size();i++){
		//���������Ч 
		int ta=f_state.idx[i],tb=f_state.rstr[i].size()-1;
		if(ta<tb){
			int tmptoken=f_state.rstr[i][f_state.idx[i]+1];//cout<<invec[tmptoken]<<" ";
			//�ҵ�ǰtokenû�м��� 
			if(!invec[tmptoken]) fcres.push_back(tmptoken),invec[tmptoken]=1;
		}
	}
	return fcres;
}

state closure(state I){
	state J=I;
	int flag=1;		//�����ж��Ƿ����µĲ���ʽ���� 
	while(flag){
		flag=0;
		int tm=J.rstr.size();	//��ǰJ�Ĵ�С�����⴦���¼���Ĳ���ʽ 
		for(int i=0;i<tm;i++){
			int next_token;
			if(J.idx[i]==J.rstr[i].size()-1){	//��������һ���ַ������ÿ����� 
				continue;
			}
			else{
				next_token=J.rstr[i][J.idx[i]+1]; 
			}
			//���ع��ķ�G�еĲ���ʽ��һ�ȶ� 
			for(int j=0;j<G.lstr.size();j++){
				//��Ҫ��֤����ʽ������next_token���Ҳ���ʽ��λ��Ϊ-1����û�б�����J�� 
				if(G.lstr[j]==next_token&&G.idx[j]==-1&&judge(G.rstr[j],J)==0){
					//cout<<"!";
					J.lstr.push_back(next_token);
					J.rstr.push_back(G.rstr[j]);
					J.idx.push_back(-1);
					flag=1;
				}
			}
		}
	}
	return J;
}

state transfer_function(state I,int next_token){
	state J;
	for(int i=0;i<I.idx.size();i++){
		//cout<<I.idx[i]<<" "<<I.rstr[i].size()-1<<endl;
		int ta=I.idx[i],tb=I.rstr[i].size()-1;
		if(ta<tb){
			if(I.rstr[i][I.idx[i]+1]==next_token){
				J.lstr.push_back(I.lstr[i]);
				J.rstr.push_back(I.rstr[i]);
				J.idx.push_back(I.idx[i]+1);
			}
		}
	}
	//cout<<J.idx.size()<<J.lstr.size()<<J.rstr.size();
	return closure(J);
} 

void proC(){
	//�Ƚ�������ʽStart->S 
	state tI;tI.lstr.push_back(0);vector<int>tvec;tvec.push_back(1);tI.rstr.push_back(tvec);tI.idx.push_back(-1);
	//�����ʼ�ıհ� 
	state I=closure(tI);
	C.push_back(I);
	//flag�����Ƿ����µ�״̬���� 
	int flag=1;
	while(flag){
		flag=0;
		/*Ϊ��ÿ�ζ�ֻ�������ϴεĽ������ȥ�����µĽ����
		ò��ûʲô���ã���ʵ���������� */ 
		int tl=C.size();	
		for(int i=0;i<tl;i++){
			//��ȡ���е��ƽ���Ŀ 
			vector<int> vec=fc(C[i]);//cout<<vec.size()<<endl;
			/*����ÿһ����ѡ��token������ת�ƺ���
			����ǿջ���û���ֹ����ͼ��뵽��Ŀ���淶����*/
			for(int j=0;j<vec.size();j++){
				state newstate;//printstate(C[i]);cout<<vec[j]<<endl;
				newstate=transfer_function(C[i],vec[j]);
				if(!newstate.idx.size()) continue;
				else if(!judge_in(newstate)){
					C.push_back(newstate);
					flag=1;
				}
			}
		} 
	}
}

void ans_table_create(){
	proC();
	/*!!!!!!!rΪ����SΪ�� !!!!!!!*/
	//������Ŀ���淶����ÿһ��״̬ 
	for(int i=0;i<C.size();i++){
		//��ÿһ��״̬�е�ÿһ������ʽ 
		for(int j=0;j<C[i].idx.size();j++){
			int tlstr=C[i].lstr[j],tidx=C[i].idx[j];
			vector<int> trstr=C[i].rstr[j];
			//��ѯ����ʽ������ 
			int next_token = judge_type(tlstr,trstr,tidx);
			//����ɼ����ƽ� 
			if(next_token>=0){
				//����ת�ƺ��� 
				state tmpstate=transfer_function(C[i],next_token);
				//�ҵ���Ҫ�ƽ�״̬���±� 
				int ttidx=find_state_idx(tmpstate);
				//����ҵ���û�в�������״̬��ֱ�ӱ��� 
				if(ttidx==-1){
					cout<<"error";return;
				}
				//�����һ��token���ս�����ͼӵ�action���У�������뵽go_to���� 
				if(terminal[itoken[next_token]]){
					action[i][next_token]=ttidx;
				}
				else{
					go_to[i][next_token]=ttidx;
				}
			}
			//�����Ŀ�����ƽ�����������ʲô���Ŷ�ֱ�ӹ�Լ 
			if(next_token==-1){
				int ttidx=-1*find_ans_idx(tlstr,trstr,tidx);
				for(int k=0;k<T.size();k++){
					action[i][token[T[k]]]=ttidx;
				}
				action[i][token.size()]=ttidx;
			}
			//���� 
			if(next_token==-2) action[i][token.size()]=acc; //acc 	
		} 
	}
}


void compile_vision(){
	for(int i=0;i<target_str.size();i++){
		string tstr1,tstr2;
		int flag=0;		//flag�����ж���token���ͻ����ַ����� 
		for(int j=0;j<target_str[i].length();j++){
			char tc=target_str[i][j];
			if(tc==9){
				flag=1;continue;
			}
			if(flag==0){
				tstr1+=tc;
			}
			else{
				tstr2+=tc;
			}
//			int prt=tc;
//			cout<<prt<<" "<<tc<<endl;
		}
		//�ֱ���뵽������ɲ����� 
		target.push_back(token[tstr1]);
		target_component.push_back(tstr2);
	}
	//������# 
	target.push_back(token.size());
	target_component.push_back("#");	//!!!!!!!!!!!!!!
}

void LR(){
	ssta.push(0);xsta.push(token.size());
	ip=0;
	while(true){
		//��ȡջ��״̬����һ��token 
		int s_top=ssta.top();
		int token_a=target[ip];
		//������һ���Ķ��� 
		int next_step=action[s_top][token_a];
//		printstate(C[s_top]);
//		cout<<s_top<<" "<<itoken[token_a]<<" "<<next_step<<endl;
		//������ƽ� 
		if(next_step>0&&next_step!=999){
			//cout<<target_component[ip]<<endl;
			//�жϵ�ǰ�ַ��Ƿ��б�Ҫ���������ҪӦѹջ���� 
			if(need_output(itoken[target[ip]])) strsta.push(target_component[ip]);//cout<<"!";
			//���·���ջ��ָ���ƽ� 
			xsta.push(target[ip]);ssta.push(next_step);ip++;
		}
		else if(next_step<0){
			next_step*=-1;
			vector<int> tstr=G.rstr[next_step];		//���������ڹ�Լ�Ĳ���ʽ 
			for(int i=0;i<tstr.size();i++) xsta.pop(),ssta.pop();
			int n_state=ssta.top();
			//���·���ջ 
			xsta.push(G.lstr[next_step]);
			ssta.push(go_to[n_state][G.lstr[next_step]]);
			//�������ʽ 
			cout<<itoken[G.lstr[next_step]]<<"-> ";
			for(int i=0;i<tstr.size();i++){
				cout<<itoken[tstr[i]]<<" ";
			}
			//������ս����Ҫ������͵�ջ 
			if(need_output(itoken[tstr[0]])) cout<<strsta.top(),strsta.pop();
			cout<<endl;
		}
		//���� 
		else if(next_step==999){
			return;
		}
		else{
			cout<<"error"<<endl;return;
		}
	}
}


int main(){
	memset(action,0,sizeof(action));
	memset(go_to,0,sizeof(go_to));
	pre1();
	G=G0;token=token0;itoken=itoken0;
	compile_vision();
	ans_table_create();
	print_ans_table();
	LR();
//	for(int i=0;i<target.size();i++) cout<<itoken[target[i]]<<" ";cout<<endl;
//	for(int i=0;i<target_component.size();i++) cout<<target_component[i]<<" ";
	
	
//	������debug 
//	for(int i=0;i<C.size();i++){
//		cout<<"state"<<i<<endl; 
//		printstate(C[i]);cout<<endl;
//	} cout<<endl<<endl<<endl;
//	printstate(G);
//	cout<<endl;
//	print_ans_table();
//	for(int i=1;i<V.size();i++) cout<<V[i]<<"   ";
//	for(int i=0;i<T.size();i++) cout<<T[i]<<"   ";
//	cout<<"#";
//	cout<<endl;
//	
//	for(int i=0;i<C.size();i++){
//		for(int j=1;j<=token.size();j++){
//			cout<<action[i][j]<<"   ";
//		}cout<<endl;
//	}cout<<endl<<endl;
//	for(int i=0;i<C.size();i++){
//		for(int j=1;j<=token.size();j++){
//			cout<<go_to[i][j]<<"   ";
//		}cout<<endl;
//	}
	
	
	
//  ��Ŀ���淶��debug
//	proC();
//	for(int i=0;i<C.size();i++){
//		printstate(C[i]);cout<<endl;
//	} 

//  ת�ƺ���debug
//	state test;
//	test.lstr.push_back(0);vector<int> tvec1;tvec1.push_back(1);test.rstr.push_back(tvec1);test.idx.push_back(-1);
//	test.lstr.push_back(2);vector<int> tvec2;tvec2.push_back(3);tvec2.push_back(2);test.rstr.push_back(tvec2);test.idx.push_back(0);
//	test.lstr.push_back(2);vector<int> tvec3;tvec3.push_back(3);tvec3.push_back(2);test.rstr.push_back(tvec3);test.idx.push_back(-1);
//	test.lstr.push_back(2);vector<int> tvec4;tvec4.push_back(4);test.rstr.push_back(tvec4);test.idx.push_back(-1);
//	state res=transfer_function(test,4);
//	for(int i=0;i<res.idx.size();i++){
//		cout<<itoken[res.lstr[i]]<<"   ";
//		if(res.idx[i]==-1) cout<<".";
//		for(int j=0;j<res.rstr[i].size();j++){
//			cout<<itoken[res.rstr[i][j]];
//			if(j==res.idx[i]) cout<<".";
//		} 
//		cout<<"  "<<res.idx[i]<<endl;
//	}



//  �հ�debug	
//	state test;
//	test.lstr.push_back(2);
//	vector<int> tvec;
//	tvec.push_back(3);tvec.push_back(2);//tvec.push_back(7);
//	test.rstr.push_back(tvec);
//	test.idx.push_back(1);
	
//	test.lstr.push_back(2);
//	vector<int> tvec2;
//	tvec2.push_back(4);//tvec.push_back(16);tvec.push_back(7);
//	test.rstr.push_back(tvec2);
//	test.idx.push_back(-1);
//	state res = closure(test);
//	for(int i=0;i<res.idx.size();i++){
//		cout<<itoken[res.lstr[i]]<<"   ";
//		if(res.idx[i]==-1) cout<<".";
//		for(int j=0;j<res.rstr[i].size();j++){
//			cout<<itoken[res.rstr[i][j]];
//			if(j==res.idx[i]) cout<<".";
//		} 
//		cout<<"  "<<res.idx[i]<<endl;
//	}

//  Ԥ����debug 
//	for(int i=0;i<G.idx.size();i++){
//		cout<<i<<" "<<itoken[G.lstr[i]]<<"   ";
//		for(int j=0;j<G.rstr[i].size();j++) cout<<itoken[G.rstr[i][j]]<<" ";
//		cout<<"     "<<G.idx[i]<<endl;
//	}
//	cout<<terminal["S"]<<terminal["B"]<<terminal["a"]<<terminal["b"]<<endl;

	return 0;
}
