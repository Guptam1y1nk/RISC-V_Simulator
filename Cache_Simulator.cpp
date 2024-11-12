#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>
using namespace std;
#define ll long long

ll registers[32];
bitset<8> mem[0x50000];
ll instruction_pos = 0;
ll free_index = 0;

int rows = 0, cols = 0, ass = 0;
ll*** cache;
int enable = 0;
int accesses = 0, hits = 0, misses = 0;
string cache_type;
int time_stamp_access = 0, time_stamp_misses = 0;

int replace_random(){
	int index = rand()%ass;
	return index;
}

int replace_LRU(int row){
	int mini = INT_MAX;
	int index = -1;

	for(int i=0;i<ass;i++){
		if(cache[row][i][3] < mini){
			mini = cache[row][i][3];
			index = i;
		}
	}

	return index;
}

int replace_FIFO(int row){
	int mini = INT_MAX;
	int index = -1;

	for(int i=0;i<ass;i++){
		if(cache[row][i][4] < mini){
			mini = cache[row][i][4];
			index = i;
		}
	}

	return index;
}

void twoComplement(string &s){
	int n = s.size()-1;
	while(s[n] != '1'){
		n--;
	}
	for(int i = n-1;i>=0;i--){
		if(s[i] == '1'){
			s[i] = '0';
		}
		else if(s[i] == '0'){
			s[i] = '1';
		}
	}
}

string convert_deci_to_binary(int n,int width){
	string s;
	bool neg = false;

	if(n<0){
		neg = true;
		n = -n;
	}

	while(n>0){
		s.push_back( (n%2) + '0');
		n = n/2;
	}
	reverse(s.begin(),s.end());
	
	int len = s.size();
	string s1(width-len,'0');
	s = s1+s;
	if(neg){
		twoComplement(s);
	}

	return s;
}

string getPC(ll PC){
	PC *= 4;
    string PC_num = "";
  
    while (PC != 0) {
        int rem = 0;
        char ch;
        rem = PC % 16;

        if (rem < 10) {
            ch = rem + 48;
        }
        else {
            ch = rem + 55;
        }
        PC_num += ch;
        PC = PC / 16;
    }
    
    int i = 0, j = PC_num.size() - 1;
    while(i <= j)
    {
      swap(PC_num[i], PC_num[j]);
      i++;
      j--;
    }

	string ans = "0x";
	for(int i=0;i<8-PC_num.size();i++){
		ans += '0';
	}
	ans += PC_num;

	return ans;
}

string toHex(long num1){
	if (num1 == 0)
		return "0";
	long num = num1;
	string s = "";
	while (num) {
		int temp = num % 16;
		if (temp <= 9)
		s += (48 + temp);
		else
		s += (55 + temp);
		num = num / 16;
	}
	reverse(s.begin(), s.end());
	return s;
}

map<string,string> instructType = {{"add", "R"}, {"sub", "R"}, {"and", "R"}, {"or", "R"}, {"xor", "R"}, {"sll", "R"}, {"srl", "R"}, {"sra", "R"}, {"addi", "I"}, {"xori", "I"}, {"ori", "I"}, {"andi", "I"}, {"slli", "I"}, {"srli", "I"}, {"srai", "I"}, {"lb", "I_l"}, {"lh", "I_l"}, {"lw", "I_l"}, {"ld", "I_l"}, {"lbu", "I_l"}, {"lhu", "I_l"}, {"lwu", "I_l"}, {"sb", "S"}, {"sh", "S"}, {"sw", "S"}, {"sd", "S"}, {"beq", "B"}, {"bne", "B"}, {"blt", "B"}, {"bge", "B"},{"bltu", "B"}, {"bgeu" , "B"}, {"jal", "J"}, {"jalr", "JR"}, {"lui", "U"}, {"auipc", "U"}};
map<string, string> alias = {{"zero", "x0"}, {"ra", "x1"}, {"sp", "x2"}, {"gp", "x3"}, {"tp", "x4"}, {"t0", "x5"}, {"t1", "x6"}, {"t2", "x7"}, {"s0", "x8"}, {"s1", "x9"}, {"a0", "x10"}, {"a1", "x11"}, {"a2", "x12"}, {"a3", "x13"}, {"a4", "x14"}, {"a5", "x15"}, {"a6", "x16"}, {"a7", "x17"}, {"s2", "x18"}, {"s3", "x19"}, {"s4", "x20"}, {"s5", "x21"}, {"s6", "x22"}, {"s7", "x23"}, {"s8", "x24"}, {"s9", "x25"}, {"s10", "x26"}, {"s11", "x27"}, {"t3", "x28"}, {"t4", "x29"}, {"t5", "x30"}, {"t6", "x31"}};
map<string, int> labels;
vector<string> text;
vector<string> codes;
unordered_set<int> breakpoints;

int getRegister(string temp){
	if(temp[0] != 'x'){
		temp = alias[temp];
	}
	int num = 0;
	for(int j=1;j<temp.length();j++){
		num = num*10 + (temp[j]-'0');
	}

	return num;
}

bool execute_Rtype(string line, int i){
	instruction_pos++;

	string op = "", rd = "", rs1 = "", rs2 = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
		rs2 += line[a];
		a++;
	}

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);
	int rd_num = getRegister(rd); 

	if (op == "add") {
		registers[rd_num] = registers[rs1_num] + registers[rs2_num];
	} else if (op == "sub") {
		registers[rd_num] = registers[rs1_num] - registers[rs2_num];
	} else if (op == "and") {
		registers[rd_num] = registers[rs1_num] & registers[rs2_num];
	} else if (op == "or") {
		registers[rd_num] = registers[rs1_num] | registers[rs2_num];
	} else if (op == "xor") {
		registers[rd_num] = registers[rs1_num] ^ registers[rs2_num];
	} else if (op == "sll") {
		registers[rd_num] = registers[rs1_num] << registers[rs2_num];
	} else if (op == "srl") {
		registers[rd_num] = registers[rs1_num] >> registers[rs2_num];
	} else if (op == "sra") {
		registers[rd_num] = registers[rs1_num] >> registers[rs2_num];
	} else {
		cout << "Invalid Instruction" << endl;
	}

	return false;
}

bool execute_Itype(string line, int i){
	instruction_pos++;
	string rd = "", rs1 = "", imm = "", op = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expected value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expected value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expected value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expected value"<<endl;
			return true;
		}
		imm += line[a];
		a++;
	}
	
	int rs1_num = getRegister(rs1);
	int rd_num = getRegister(rd);
	
	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}
	
	if(op == "addi"){
		registers[rd_num] = registers[rs1_num] + num;
	} else if(op == "andi"){
		registers[rd_num] = registers[rs1_num] & num;
	} else if(op == "ori"){
		registers[rd_num] = registers[rs1_num] | num;
	} else if(op == "slli"){
		registers[rd_num] = registers[rs1_num] << num;
	} else if(op == "srli"){
		registers[rd_num] = registers[rs1_num] >> num;
	} else if(op == "srai"){
		registers[rd_num] = registers[rs1_num] >> num;
	}

	return false;
}

bool execute_I_ltype(string line, int i){
	instruction_pos++;
	string rd = "", rs1 = "", imm = "", op = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != '('){
		imm += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;

	while(line[a] != ')'){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	if(a != line.length()-1){
		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
		return true;
	}

	int rs1_num = getRegister(rs1);
	int rd_num = getRegister(rd);

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}

	ll pos = registers[rs1_num] + num - 0x10000;
	ll data = 0;

	if(op=="lb" || op=="lbu"){
		long byte;
		for(int i=0;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}
	else if(op=="lh" || op=="lhu"){
		long byte = 0;
		for(int i=1;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}
	else if(op=="lw" || op=="lwu"){
		long byte = 0;
		for(int i=3;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}
	}
	else if(op=="ld"){
		accesses++;
		time_stamp_access++;
		int address = registers[rs1_num] + num;
		int col_num = address & (cols-1);
		int offset_bits = log2(cols);
		address = address >> offset_bits;	
		int row_num = address & (rows-1);
		int index_bits = log2(rows);
		address = address >> index_bits;
		int tag = address;		

		if(enable==1 && (cols - col_num) >= 8){
			for(int i=0;i<ass;i++){
				if(tag == cache[row_num][i][0] && cache[row_num][i][1] == 1){
					hits++;
					cache[row_num][i][3] = time_stamp_access;
					long byte = 0;
					for(int j=7;j>=0;j--){
						byte = cache[row_num][i][col_num + 5 + j];
						data += byte;
						if(j>0){
							data = data << 8;
						}
					}

					registers[rd_num] = data;
					return false;
				}
			}
		}

		misses++;
		time_stamp_misses++;
		long byte = 0;
		for(int i=7;i>=0;i--){
			byte = mem[pos+i].to_ulong();
			data += byte;
			if(i>0){
				data = data << 8;
			}
		}

		if(col_num == 0){
			int temp = 5, replace = 1;
			for(int i=0;i<ass;i++){
				if(cache[row_num][i][1] == 0){
					replace = 0;
					cache[row_num][i][3] = time_stamp_access;
					cache[row_num][i][4] = time_stamp_misses;
					cache[row_num][i][1] = 1;
					for(int j=0;j<cols;j++){
						byte = mem[pos+j].to_ulong();
						cache[row_num][i][temp++] = byte;
					}
					cache[row_num][i][0] = tag;
					break;
				}
			}

			if(replace){
				int index = 0;
				if(cache_type == "RANDOM"){
					index = replace_random();
				}
				else if(cache_type == "LRU"){
					index = replace_LRU(row_num);
				}
				else if(cache_type == "FIFO"){
					index = replace_FIFO(row_num);
				}

				cache[row_num][index][3] = time_stamp_access;
				cache[row_num][index][4] = time_stamp_misses;
				for(int j=0;j<cols;j++){
					byte = mem[pos+j].to_ulong();
					cache[row_num][index][temp++] = byte;
				}
				cache[row_num][index][0] = tag;
			}
		}
	}

	registers[rd_num] = data;
	return false;
}

bool execute_Stype(string line, int i){
	instruction_pos++;
	string op="",rs2="",rs1="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rs2 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(line[a] != '('){
		imm += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
	while(line[a] != ')' ){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	if(a != line.length()-1){
		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
		return true;
	}

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return true;
	}

	ll pos = registers[rs1_num] + num - 0x10000;
	ll data = registers[rs2_num];

	if(op=="sb"){
		int byte = 0;
		for(int i=1;i<=1;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else if(op=="sh"){
		int byte = 0;
		for(int i=1;i<=2;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else if(op=="sw"){
		int byte = 0;
		for(int i=1;i<=4;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else if(op=="sd"){
		int byte = 0;
		for(int i=1;i<=8;i++){
			byte = (data & (0b11111111));
			mem[pos++] = byte; 
			data = data >> 8;
		}
	}
	else{
		cout<<"Invalid Instruction"<<endl;
	}

	return false;
}

int unsignedComp(int n, int m){
	string n_unsigned = convert_deci_to_binary(n,16);
	string m_unsigned = convert_deci_to_binary(m,16);

	int size = n_unsigned.size();
	for(int i=0;i<size;i++){
		if(n_unsigned[i] == '0' && m_unsigned[i] == '1'){
			return 1;
		}
		if(n_unsigned[i] == '1' && m_unsigned[i] == '0'){
			return 0;
		}
	}
	return 0;
}

pair<int, bool> execute_Btype(string line, int i){
	string op="",rs2="",rs1="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a++;
		
	while(line[a] != ','){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a += 2;

	while(line[a] != ','){
		rs2 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a += 2;

	while(a < line.length()){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return {-1, true};
		}
		imm += line[a];
		a++;
	}

	if(labels.find(imm) == labels.end()){
		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
		return {-1, true};
	}

	int rs1_num = getRegister(rs1);
	int rs2_num = getRegister(rs2);
	int jump_index = labels[imm];
	int diff = (labels[imm] - i)*4;

	if(rs1_num < 0 || rs1_num>31 || rs2_num < 0 || rs2_num>31 ){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return {-1, true};
	}

	if(diff < -4096 || diff > 4095){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return {-1, true};
	}
	
	if(op == "beq" && registers[rs1_num] == registers[rs2_num]){
		instruction_pos = jump_index;
	} else if(op == "bne" && registers[rs1_num] != registers[rs2_num]){
		instruction_pos = jump_index;
	} else if(op == "blt" && registers[rs1_num] < registers[rs2_num]){
		instruction_pos = jump_index;
	} else if(op == "bge" && registers[rs1_num] >= registers[rs2_num]){
		instruction_pos = jump_index;
	} else if(op == "bltu" && (unsignedComp(registers[rs1_num] , registers[rs2_num]) == 1) ){
		instruction_pos = jump_index;
	} else if(op == "bgeu" && (unsignedComp(registers[rs1_num] , registers[rs2_num]) == 0) ){
		instruction_pos = jump_index;
	} else{
		instruction_pos++;
	}

	return {instruction_pos, false};
}

pair<int,bool> execute_Jtype(string line, int i){
	string op="",rd="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1,true};
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1,true};
		}
	}
	a += 2;

	while(a < line.length() ){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return {-1,true};
		}
		imm += line[a];
		a++;
	}

	if(labels.find(imm) == labels.end()){
		cout<<"Error found in line "<<i+1<<": Label not defined"<<endl;
		return {-1,true};
	}
	int diff = labels[imm] - i;
	diff *= 4;
	int jump_index = labels[imm];

	int rd_num = getRegister(rd);
	if(rd_num < 0 || rd_num>31){
		cout<<"Error found in line "<<i+1<<": This code doesn't support more than 32 Registers"<<endl;
		return {-1,true};
	}

	if(op == "jal"){
		registers[rd_num] = instruction_pos + 1;
		instruction_pos = jump_index;
	}
	return {instruction_pos,false};
}

pair<int, bool> execute_jalr(string line, int i){
	string rd = "", rs1 = "", imm = "", op = "";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a++;

	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a += 2;

	while(line[a] != '('){
		imm += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	a++;

	while(line[a] != ')'){
		rs1 += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return {-1, true};
		}
	}
	if(a != line.length()-1){
		cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
		return {-1, true};
	}

	int rs1_num = getRegister(rs1);
	int rd_num = getRegister(rd);

	int num = stoi(imm);
	if(num < -2048 || num > 2047){
		cout<<"Error found in line "<<i+1<<": Immediate value lies outside the allowed range"<<endl;
		return {-1, true};
	}

	int des = registers[rs1_num] + num;
	instruction_pos = des;

	return {instruction_pos, false};
}

ll getAddress(string imm){
	ll address = 0;
	int n = imm.length();

	for(int i=2;i<n;i++){
		int val = 0;
		if(imm[i]>='0' && imm[i]<='9'){
			val = imm[i] - '0';
		}
		else{
			val = (imm[i] - 'a') + 10;
		}
		address = address*16 + val;
	}

	return address;
}

bool execute_Utype(string line, int i){
	instruction_pos++;
	string op="",rd="",imm="";
	int a = 0;
	while(line[a] != ' '){
		op += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a++;
		
	while(line[a] != ','){
		rd += line[a];
		a++;
		if(a == line.length()){
			cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
			return true;
		}
	}
	a += 2;

	while(a < line.length() ){
		if(line[a] == ','){
			cout<<"Error found in line "<<i+1<<": Number of operands exceed the expacted value"<<endl;
			return true;
		}
		imm += line[a];
		a++;
	}

	ll num = getAddress(imm);
	int rd_num = getRegister(rd);
	num *= pow(16, 3);

	if(op=="lui"){
		registers[rd_num] = num;
	}
	else if(op=="auipc"){
		num += (instruction_pos*4);
		registers[rd_num] = num;
	}

	return false;
}

string getWord(string line){
	int i = 0, n = line.length();
	string operation;
	while(i < n && line[i] != ' '){
		operation += line[i];
		i++;
	}

	return operation;
}

void loadCommand(string filename){
	memset(registers, 0, 32*sizeof(registers[0]));
	memset(mem, 0, 0x40000*sizeof(mem[0]));
	ifstream inputFile(filename);
    string line;
	text.clear();
    while(getline(inputFile,line)){
		int i = 0, n = line.length();
		string word = "";
		while(i < n && line[i] != ' '){
			word += line[i];
			i++;
		}
		if(word==".dword"){
			line = line.substr(i);
			vector<string> v;
		
			stringstream ss(line);
		
			while (ss.good()) {
				string substr;
				getline(ss, substr, ',');
				v.push_back(substr);
			}
			for (size_t i = 0; i < v.size(); i++){
				v[i] = v[i].substr(1);
				int num = stoi(v[i]);

				for(int j=0;j<=7;j++){
					mem[free_index++] = num & (0b11111111);
					num = num >> 8;
				}
			}
		}
		else if(word==".data" || word==".text" || word==""){
			continue;
		}
		else{
			int i = 0;
			while(line[i] == ' '){
				i++;
			}
			line = line.substr(i);
			text.push_back(line);
		}
    }
    inputFile.close();

	for(int i=0;i<text.size();i++){
		string line = text[i];

		for(int j=0;j<line.length();j++){
			if(line[j] == ':'){
				string l = line.substr(0, j);
				if(labels.find(l) != labels.end()){
					cout<<"Error found in line "<<i+1<<": Label is already declared and can't be declared twice"<<endl;
					return;
				}

				labels[l] = i;

				int n = line.length();
				text[i] = line.substr(j+2, n-j-2);
				break;
			}
		}
	}
}

void makeCache(string filename){
	ifstream inputFile(filename);
    string line;
	int i = 0;
	int cache_size = 0, block_size = 0, cache_ass = 0;

    while(getline(inputFile,line)){
		switch(i){
			case 0:
				cache_size = stoi(line);
				break;
			case 1:
				block_size = stoi(line);
				break;
			case 2:
				cache_ass = stoi(line);
				break;
			case 3:
				cache_type = line;
				break;
			case 4:

				break;
		}
		i++;
	}

	rows = cache_size / block_size;
	rows /= cache_ass;
	cols = block_size;
	ass = cache_ass;

	cache = new ll**[rows];
	for(int j=0;j<rows;j++){
		cache[j] = new ll*[ass];
	}
	for(int j=0;j<rows;j++){
		for(int k=0;k<ass;k++){
			cache[j][k] = new ll[cols+5];
		}
	}

	for(int j=0;j<rows;j++){
		for(int k=0;k<ass;k++){
			for(int l=0;l<cols;l++){
				cache[j][k][l] = 0;
			}
		}
	}
}

int main(){
	while(1){
		string command;
		getline(cin, command);

		int i = 0, n = command.length();
		string operation;
		while(i < n && command[i] != ' '){
			operation += command[i];
			i++;
		}

		if(operation == "cache_sim"){
			string cache_operation;
			i++;
			while(i < n && command[i] != ' '){
				cache_operation += command[i++];
			}
			i++;

			if(cache_operation == "enable"){
				string filename = command.substr(i);
				makeCache(filename);
				enable = 1;
			}
			else if(cache_operation == "disable"){
				enable = 0;
			}
			else if(cache_operation == "status"){
				int c_size = rows*cols*ass;

				cout<<"Cache Size: "<<c_size<<endl;
				cout<<"Block Size: "<<cols<<endl;
				cout<<"Associativity: "<<ass<<endl;
				cout<<"Replacement Policy: "<<cache_type<<endl;
				cout<<endl;
			}
			else if(cache_operation == "invalidate"){
				for(int i=0;i<rows;i++){
					for(int j=0;j<ass;j++){
						cache[i][j][1] = 0;
					}
				}
			}
			else if(cache_operation == "dump"){
				string filename = command.substr(i);
			}
			else if(cache_operation == "stats"){
				double hit_rate = (double)hits/accesses;
				cout<<"D-cache statistics: Accesses="<<accesses<<", Hit="<<hits<<", Miss="<<misses<<", Hit Rate="<<hit_rate<<endl;
			}
		}
		else if(operation == "load"){
			string filename = command.substr(i+1, n-i-1);
			loadCommand(filename);
			cout<<endl;
			instruction_pos = 0;
		}
		else if(operation == "run"){
			if(instruction_pos == text.size()){
				cout<<"Nothing to run"<<endl<<endl;
				continue;
			}
			for(int i=instruction_pos;i<text.size();i++){
				if(breakpoints.find(i) != breakpoints.end()){
					cout<<"Execution stopped at breakpoint"<<endl;
					break;
				}
				string line = text[i];
				cout<<"Executed "<<line<<"; PC="<<getPC(i)<<endl;
				string op = "";
				int a = 0;
				while(line[a] != ' '){
					op += line[a];
					a++;
					if(a == line.length()){
						cout<<"Error found in line "<<i+1<<": Number of operands are less the expacted value"<<endl;
						return true;
					}
				}

				string type = instructType[op];
				if(type=="R"){
					if(execute_Rtype(line, i)){
						return true;
					}
				}
				else if(type == "S"){
					if(execute_Stype(line, i)){
						return true;
					}
				}
				else if(type=="I"){
					if(execute_Itype(line, i)){
						return true;
					}
				}
				else if(type=="I_l"){
					if(execute_I_ltype(line, i)){
						return true;
					}
				}
				else if(type=="B"){
					pair<int, bool> ans = execute_Btype(line, i);
					if(ans.second){
						return true;
					}
					else{
						i = ans.first - 1;
					}
				}
				else if(type=="J"){
					pair<int, bool> ans = execute_Jtype(line, i);
					if(ans.second){
						return true;
					}
					else{
						i = ans.first - 1;
					}
				}
				else if(type=="U"){
					if(execute_Utype(line, i)){
						return true;
					}
				}
				else if(type=="JR"){
					pair<int, bool> ans = execute_jalr(line, i);
					if(ans.second){
						return true;
					}
					else{
						i = ans.first - 1;
					}
				}
			}
			cout<<endl;
		}
		else if(operation == "step"){
			if(instruction_pos == text.size()){
				cout<<"Nothing to step"<<endl<<endl;
			}
			else{
				string line = text[instruction_pos];
				cout<<"Executed "<<line<<"; PC="<<getPC(instruction_pos)<<endl;
				string op = "";
				int a = 0;
				while(line[a] != ' '){
					op += line[a];
					a++;
					if(a == line.length()){
						cout<<"Error found in line "<<instruction_pos+1<<": Number of operands are less the expacted value"<<endl;
						return true;
					}
				}

				string type = instructType[op];
				if(type=="R"){
					if(execute_Rtype(line, instruction_pos)){
						return true;
					}
				}
				else if(type == "S"){
					if(execute_Stype(line, instruction_pos)){
						return true;
					}
				}
				else if(type=="I"){
					if(execute_Itype(line, instruction_pos)){
						return true;
					}
				}
				else if(type=="I_l"){
					if(execute_I_ltype(line, instruction_pos)){
						return true;
					}
				}
				else if(type=="B"){
					pair<int, bool> ans = execute_Btype(line, i);
					if(ans.second){
						return true;
					}
					else{
						instruction_pos = ans.first;
					}
				}
				else if(type=="J"){
					pair<int, bool> ans = execute_Jtype(line, i);
					if(ans.second){
						return true;
					}
					else{
						instruction_pos = ans.first;
					}
				}
				else if(type=="U"){
					if(execute_Utype(line, i)){
						return true;
					}
				}
				else if(type=="JR"){
					pair<int, bool> ans = execute_jalr(line, i);
					if(ans.second){
						return true;
					}
					else{
						i = ans.first - 1;
					}
				}
				cout<<endl;
			}
		}
		else if(operation == "mem"){
			string temp = "";
			i++;
			while(i<n && command[i] != ' '){
				temp += command[i++];
			}
			int address = getAddress(temp);

			temp = "";
			i++;
			while(i<n){
				temp += command[i++];
			}
			int num = stoi(temp);

			int start = address - 0x10000;
			for(int i=0;i<num;i++){
				cout<<"Memory[0x";
				long long val = address + i;
				stringstream ss;
				ss << hex << val;
				string res(ss.str());
				cout<<res<<"] = 0x";

				long byte = mem[start+i].to_ulong();
				string hexval = toHex(byte);
				cout<<hexval<<endl;
			}
			cout<<endl;
		}
		else if(operation == "regs"){
			cout<<"Registers:"<<endl;
			for(int i=0;i<32;i++){
				long long val = registers[i];
				stringstream ss;
				ss << hex << val;
				string res(ss.str());
				cout<<"x"<<i<<" = 0x"<<res<<endl;
			}
			cout<<endl;
		}
		else if(operation=="break"){
			string temp = command.substr(i+1);
			int line_num = stoi(temp)-1;

			breakpoints.insert(line_num);
			cout<<"Breakpoint set at line "<<line_num+1<<endl<<endl;
		}
		else if(operation=="del"){
			i++;
			while(command[i] != ' '){
				i++;
			}

			string temp = command.substr(i+1);
			int line_num = stoi(temp)-1;

			if(breakpoints.find(line_num) != breakpoints.end()){
				breakpoints.erase(line_num);
				cout<<"Breakpoint removed from line "<<line_num+1<<endl<<endl;
			}
			else{
				cout<<"Breakpoint at this line does not exist"<<endl<<endl;
			}
		}
		else if(operation == "exit"){
			cout<<"Exited the simulator"<<endl<<endl;
			break;
		}
		else{
			cout<<"Invalid Operation"<<endl<<endl;
			break;
		}
	}

	return 0;
}
