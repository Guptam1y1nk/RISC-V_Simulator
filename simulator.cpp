#include <bits/stdc++.h>

using namespace std;

string convert_deci_to_binary(int n){
	string s;
	while(n>0){
		s.push_back( (n%2) + '0');
		n = n/2;
	}
	reverse(s.begin(),s.end());
	int len = s.size();
	string s1(5-len,'0');
	s = s1+s;
	return s;
}

void convert_hex_to_binary(map<char,string> &hex_to_binary){
	hex_to_binary['0'] = "0000";
	hex_to_binary['1'] = "0001";
	hex_to_binary['2'] = "0010";
	hex_to_binary['3'] = "0011";
	hex_to_binary['4'] = "0100";
	hex_to_binary['5'] = "0101";
	hex_to_binary['6'] = "0110";
	hex_to_binary['7'] = "0111";
	hex_to_binary['8'] = "1000";
	hex_to_binary['9'] = "1001";
	hex_to_binary['a'] = "1010";
	hex_to_binary['b'] = "1011";
	hex_to_binary['c'] = "1100";
	hex_to_binary['d'] = "1101";
	hex_to_binary['e'] = "1110";
	hex_to_binary['f'] = "1111";
}

void convert_binary_to_hex(map<string,char> &binary_to_hex){
	binary_to_hex["0000"] = '0';
	binary_to_hex["0001"] = '1';
	binary_to_hex["0010"] = '2';
	binary_to_hex["0011"] = '3';
	binary_to_hex["0100"] = '4';
	binary_to_hex["0101"] = '5';
	binary_to_hex["0110"] = '6';
	binary_to_hex["0111"] = '7';
	binary_to_hex["1000"] = '8';
	binary_to_hex["1001"] = '9';
	binary_to_hex["1010"] = 'a';
	binary_to_hex["1011"] = 'b';
	binary_to_hex["1100"] = 'c';
	binary_to_hex["1101"] = 'd';
	binary_to_hex["1110"] = 'e';
	binary_to_hex["1111"] = 'f';
}

void find_instructType(map<string,string> &instruct){
	instruct["add"] = "R"; 
	instruct["sub"] = "R"; 
	instruct["and"] = "R"; 
	instruct["or"] = "R"; 
	instruct["xor"] = "R"; 
	instruct["sll"] = "R"; 
	instruct["srl"] = "R"; 
	instruct["sra"] = "R"; 

	instruct["addi"] = "I";
	instruct["xori"] = "I";
	instruct["ori"] = "I";
	instruct["andi"] = "I";
	instruct["slli"] = "I";
	instruct["srli"] = "I";
	instruct["srai"] = "I";

	instruct["lb"] = "I_l";
	instruct["lh"] = "I_l";
	instruct["lw"] = "I_l";
	instruct["ld"] = "I_l";
	instruct["lbu"] = "I_l";
	instruct["lhu"] = "I_l";
	instruct["lwu"] = "I_l";

	instruct["sb"] = "S";
	instruct["sh"] = "S";
	instruct["sw"] = "S";
	instruct["sd"] = "S";
}

void find_opcode(map<string,string> &instruct_opcode){
	instruct_opcode["R"] = "0110011";
	instruct_opcode["I"] = "0010011";
	instruct_opcode["I_l"] = "0000011";
	instruct_opcode["S"] = "0100011";
	instruct_opcode["B"] = "1100011";
	instruct_opcode["J"] = "1101111";
	instruct_opcode["U"] = "0110111";
}

void find_funct3(map<string,string> &instructFunct3){
	instructFunct3["add"] = "000"; 
	instructFunct3["sub"] = "000"; 
	instructFunct3["and"] = "111"; 
	instructFunct3["or"] = "110"; 
	instructFunct3["xor"] = "100"; 
	instructFunct3["sll"] = "001"; 
	instructFunct3["srl"] = "101"; 
	instructFunct3["sra"] = "101"; 

	instructFunct3["addi"] = "000";
	instructFunct3["xori"] = "100";
	instructFunct3["ori"] = "110";
	instructFunct3["andi"] = "111";
	instructFunct3["slli"] = "001";
	instructFunct3["srli"] = "101";
	instructFunct3["srai"] = "101";

	instructFunct3["lb"] = "000";
	instructFunct3["lh"] = "001";
	instructFunct3["lw"] = "010";
	instructFunct3["ld"] = "011";
	instructFunct3["lbu"] = "100";
	instructFunct3["lhu"] = "101";
	instructFunct3["lwu"] = "110";

	instructFunct3["sb"] = "000";
	instructFunct3["sh"] = "001";
	instructFunct3["sw"] = "010";
	instructFunct3["sd"] = "011";

}

void find_funct7(map<string,string> &instructFunct7){
	instructFunct7["add"] = "000"; 
	instructFunct7["sub"] = "000"; 
	instructFunct7["and"] = "111"; 
	instructFunct7["or"] = "110"; 
	instructFunct7["xor"] = "100"; 
	instructFunct7["sll"] = "001"; 
	instructFunct7["srl"] = "101"; 
	instructFunct7["sra"] = "101"; 

	instructFunct7["addi"] = "000";
	instructFunct7["xori"] = "100";
	instructFunct7["ori"] = "110";
	instructFunct7["andi"] = "111";
	instructFunct7["slli"] = "001";
	instructFunct7["srli"] = "101";
	instructFunct7["srai"] = "101";

	instructFunct7["lb"] = "000";
	instructFunct7["lh"] = "001";
	instructFunct7["lw"] = "010";
	instructFunct7["ld"] = "011";
	instructFunct7["lbu"] = "100";
	instructFunct7["lhu"] = "101";
	instructFunct7["lwu"] = "110";

	instructFunct7["sb"] = "000";
	instructFunct7["sh"] = "001";
	instructFunct7["sw"] = "010";
	instructFunct7["sd"] = "011";

}

map<int,string> deci_to_binary;
map<char,string> hex_to_binary;
map<string,char> binary_to_hex;
map<string,string> instructType;
map<string,string> instruct_opcode;
map<string,string> instructFunct3;
map<string,string> instructFunct7;

string convert_32bits_to_hex(string s){
	string ans = "";
	for(int i=0;i<32; i += 4){
		string r = s.substr(i,4);
		ans += binary_to_hex[r];
	}
	return ans;
}

int main(){
	for(int i=0;i<32;i++){
		deci_to_binary[i] = convert_deci_to_binary(i);
		//cout<<i<<" -> "<<deci_to_binary[i];
		//cout<<endl;
	}
	convert_hex_to_binary(hex_to_binary);
	convert_binary_to_hex(binary_to_hex);
	find_instructType(instructType);
	find_opcode(instruct_opcode);
	find_funct3(instructFunct3);

	string s(32,'0');
	cout<<convert_32bits_to_hex(s)<<endl;
	//cout<<instruct_opcode[instruct["and"]]<<endl;
	return 0;
}
