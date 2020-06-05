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
state G;													//拓广文法 
vector<state> C;											//项目集规范族 
vector<int> target;											//待分析语句的组成部分 int型 
vector<string> target_component;							//待分析语句的组成部分 string型 

map<string,int> token;										//根据token直接查找它的下标 
map<int,string> itoken;										//根据token的下标重新映射回字符串形式 

stack<int> ssta;											//状态栈 
stack<int> xsta;											//符号栈，用于存放活前缀 
stack<string> strsta;										//用于存放需要输出的终结符 

int action[100][100],go_to[100][100];						//action和go_to表 
int ip;														//指向下一个待分析符号 

void printstate(state res);									//打印一个state的内容 
void print_ans_table();										//打印分析表 
bool judge(vector<int> &j_str, state j_state);				//判断产生式是否已经在state中（仅针对位置为-1） 
bool judge_equal(vector<int> &vec1,vector<int> &vec2);		//判断两个vector是否相等，用于判断产生式右部是否相等 
bool judge_in(state j_state);								//判断新的state是否已经在当前项目集规范族C中 
int judge_type(int j_lstr,vector<int> &j_rstr,int j_idx);	//判断产生式的类型，专用于分析表构造 
int find_ans_idx(int f_lstr,vector<int> &f_rstr,int f_idx);	//在拓广文法G中获取产生式下标 
int find_state_idx(state f_state);							//获取状态的下标 
bool need_output(string tt);								//是否是需要输出的终结符（目前仅对于实验二的标准文法有效） 
vector<int> fc(state f_state);								//判断一个状态可以有多少个移进选项，并以vector形式返回 
state closure(state I);										//求一个状态的闭包 
state transfer_function(state I,int next_token);			//求转移函数 
void proC();												//求项目集规范族C 
void ans_table_create(); 									//生成分析表 
void compile_vision();										//用于识别待分析语句 
void LR();													//语法分析程序 


void printstate(state res){
	//依次打印状态中的内容 
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
			//999特指分析结束 
			if(action[i][j]==999) cout<<"end";
			else cout<<action[i][j];
		} cout<<"|";
		for(int j=1;j<V.size();j++) cout.width(wid),cout<<go_to[i][j];
		cout<<endl;
	} 
}

bool judge(vector<int> &j_str, state j_state){
	//遍历J的每一个产生式与待加入的产生式比对 
	for(int i=0;i<j_state.rstr.size();i++){
		int flag=0;		//用于标记是否相同0相同，1不同 
		if(j_state.idx[i]!=-1) continue;	//如果当前产生式的位置不是-1，就直接跳过 
		for(int j=0;j<j_str.size();j++){	//逐一比对 
			if((j_str[j]!=j_state.rstr[i][j])||(j==j_state.rstr[i].size())){
				flag=1;break;
			}
		}
		//全部相同且长度也相同 
		if(flag==0&&j_str.size()==j_state.rstr[i].size()) return true;	//出现过 
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
		//对于新状态中的每一项 
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
	//如果产生式是Start->S. 
	if(j_lstr==0&&j_rstr[0]==1&&j_idx==0) return -2;
	//如果产生式是A->alpha. 
	if(j_idx==j_rstr.size()-1) return -1;
	else{
		//返回下一个token的编号 
		return j_rstr[j_idx+1];
	}
}

int find_ans_idx(int f_lstr,vector<int> &f_rstr,int f_idx){
	//遍历G中所有的产生式 
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
		//对于新状态中的每一项 
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
	//对于每个产生式 
	for(int i=0;i<f_state.idx.size();i++){
		//如果长度有效 
		int ta=f_state.idx[i],tb=f_state.rstr[i].size()-1;
		if(ta<tb){
			int tmptoken=f_state.rstr[i][f_state.idx[i]+1];//cout<<invec[tmptoken]<<" ";
			//且当前token没有加入 
			if(!invec[tmptoken]) fcres.push_back(tmptoken),invec[tmptoken]=1;
		}
	}
	return fcres;
}

state closure(state I){
	state J=I;
	int flag=1;		//用于判断是否有新的产生式加入 
	while(flag){
		flag=0;
		int tm=J.rstr.size();	//当前J的大小，避免处理新加入的产生式 
		for(int i=0;i<tm;i++){
			int next_token;
			if(J.idx[i]==J.rstr[i].size()-1){	//如果是最后一个字符，不用考虑了 
				continue;
			}
			else{
				next_token=J.rstr[i][J.idx[i]+1]; 
			}
			//与拓广文法G中的产生式逐一比对 
			for(int j=0;j<G.lstr.size();j++){
				//需要保证产生式的左部是next_token，且产生式的位置为-1，且没有被归入J中 
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
	//先建立产生式Start->S 
	state tI;tI.lstr.push_back(0);vector<int>tvec;tvec.push_back(1);tI.rstr.push_back(tvec);tI.idx.push_back(-1);
	//求最初始的闭包 
	state I=closure(tI);
	C.push_back(I);
	//flag代表是否有新的状态加入 
	int flag=1;
	while(flag){
		flag=0;
		/*为了每次都只遍历到上次的结果，不去处理新的结果，
		貌似没什么大用，但实在懒得想了 */ 
		int tl=C.size();	
		for(int i=0;i<tl;i++){
			//获取所有的移进项目 
			vector<int> vec=fc(C[i]);//cout<<vec.size()<<endl;
			/*对于每一个可选的token，计算转移函数
			如果非空或者没出现过，就加入到项目集规范族中*/
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
	/*!!!!!!!r为负，S为正 !!!!!!!*/
	//遍历项目集规范族中每一个状态 
	for(int i=0;i<C.size();i++){
		//对每一个状态中的每一个产生式 
		for(int j=0;j<C[i].idx.size();j++){
			int tlstr=C[i].lstr[j],tidx=C[i].idx[j];
			vector<int> trstr=C[i].rstr[j];
			//查询产生式的类型 
			int next_token = judge_type(tlstr,trstr,tidx);
			//如果可继续移进 
			if(next_token>=0){
				//计算转移函数 
				state tmpstate=transfer_function(C[i],next_token);
				//找到需要移进状态的下标 
				int ttidx=find_state_idx(tmpstate);
				//如果找到了没有产生过的状态，直接报错 
				if(ttidx==-1){
					cout<<"error";return;
				}
				//如果下一个token是终结符，就加到action表中，否则加入到go_to表中 
				if(terminal[itoken[next_token]]){
					action[i][next_token]=ttidx;
				}
				else{
					go_to[i][next_token]=ttidx;
				}
			}
			//如果项目不可移进，不管遇到什么符号都直接规约 
			if(next_token==-1){
				int ttidx=-1*find_ans_idx(tlstr,trstr,tidx);
				for(int k=0;k<T.size();k++){
					action[i][token[T[k]]]=ttidx;
				}
				action[i][token.size()]=ttidx;
			}
			//结束 
			if(next_token==-2) action[i][token.size()]=acc; //acc 	
		} 
	}
}


void compile_vision(){
	for(int i=0;i<target_str.size();i++){
		string tstr1,tstr2;
		int flag=0;		//flag用于判定是token类型还是字符内容 
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
		//分别加入到语句的组成部分中 
		target.push_back(token[tstr1]);
		target_component.push_back(tstr2);
	}
	//最后加入# 
	target.push_back(token.size());
	target_component.push_back("#");	//!!!!!!!!!!!!!!
}

void LR(){
	ssta.push(0);xsta.push(token.size());
	ip=0;
	while(true){
		//获取栈顶状态和下一个token 
		int s_top=ssta.top();
		int token_a=target[ip];
		//分析下一步的动作 
		int next_step=action[s_top][token_a];
//		printstate(C[s_top]);
//		cout<<s_top<<" "<<itoken[token_a]<<" "<<next_step<<endl;
		//如果是移进 
		if(next_step>0&&next_step!=999){
			//cout<<target_component[ip]<<endl;
			//判断当前字符是否有必要输出，若需要应压栈备用 
			if(need_output(itoken[target[ip]])) strsta.push(target_component[ip]);//cout<<"!";
			//更新分析栈，指针移进 
			xsta.push(target[ip]);ssta.push(next_step);ip++;
		}
		else if(next_step<0){
			next_step*=-1;
			vector<int> tstr=G.rstr[next_step];		//接下来用于规约的产生式 
			for(int i=0;i<tstr.size();i++) xsta.pop(),ssta.pop();
			int n_state=ssta.top();
			//更新分析栈 
			xsta.push(G.lstr[next_step]);
			ssta.push(go_to[n_state][G.lstr[next_step]]);
			//输出产生式 
			cout<<itoken[G.lstr[next_step]]<<"-> ";
			for(int i=0;i<tstr.size();i++){
				cout<<itoken[tstr[i]]<<" ";
			}
			//如果有终结符需要输出，就弹栈 
			if(need_output(itoken[tstr[0]])) cout<<strsta.top(),strsta.pop();
			cout<<endl;
		}
		//结束 
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
	
	
//	分析表debug 
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
	
	
	
//  项目集规范族debug
//	proC();
//	for(int i=0;i<C.size();i++){
//		printstate(C[i]);cout<<endl;
//	} 

//  转移函数debug
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



//  闭包debug	
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

//  预处理debug 
//	for(int i=0;i<G.idx.size();i++){
//		cout<<i<<" "<<itoken[G.lstr[i]]<<"   ";
//		for(int j=0;j<G.rstr[i].size();j++) cout<<itoken[G.rstr[i][j]]<<" ";
//		cout<<"     "<<G.idx[i]<<endl;
//	}
//	cout<<terminal["S"]<<terminal["B"]<<terminal["a"]<<terminal["b"]<<endl;

	return 0;
}
