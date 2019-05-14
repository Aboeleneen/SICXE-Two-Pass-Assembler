/*
 * parser.h
 *	parser of pass1 sic assembler
 *  Created on: Apr 21, 2019
 *  Author: mahmoud sharshar
 */

#ifndef PARSER_H_//guard ,created for us
#define PARSER_H_

#include <map>
#include <string>
#include <utility>
#include<fstream>
#include <vector>
#include <regex>
#include <iostream>
#include<cmath>
#include <cctype>
#include <string.h>

using namespace std;
//operation code informatio
struct opCodeInfo {
	string code="";
	int format;
	int numOfOperand;
};
//label information
struct labelInfo {
	string address="";
	bool flag;
};
//parts of each statement
struct statParts {
	string label = "";
	string opCode = "";
	string operand = "";
	string comment = "";
	bool commentOnly;
};

class Pass1 {
public:
	int lenghtOfProgram;
	int locCtr;
	int lineNumber;
	int startingAddress;
    bool noError ;
	map<string, opCodeInfo> opCodeTable;
	map<string, labelInfo> symTable;
	//extract parts of each statement using regex
	statParts parseLine(string line) {
		statParts parts;
		regex comment("\\..+");
		smatch match;
		if (regex_search(line, match, comment)) {
			parts.comment = match[0];
			parts.commentOnly = true;
		}
		string s = regex_replace(line, comment, "");
		regex m1(
				"\\s*([^\\s]+)\\s+(\\+?\\w+)\\s+([\\#\\@\\*\\'[:d:][:w:],X]+)\\s*",
				regex_constants::icase);
		regex m2("\\s*(\\+?\\w+)\\s+([\\#\\@\\*[:d:][:w:],X]+)\\s*",
				regex_constants::icase);
		regex m3("\\s*(\\w+)\\s*", regex_constants::icase);
		regex start("\\s*([^\\s]+)\\s+(start|end)\\s*", regex_constants::icase);
		smatch s1;
		smatch s2;
		smatch s3;
		smatch s4;
		if (regex_search(s, s1, m1)) {
			parts.label = s1[1];
			parts.opCode = s1[2];
			parts.operand = s1[3];
			parts.commentOnly = false;
		} else if (regex_search(s, s4, start)) {
			parts.opCode = s4[2];
			parts.label = s4[1];
			parts.commentOnly = false;
		} else if (regex_search(s, s2, m2)) {
			parts.opCode = s2[1];
			parts.operand = s2[2];
			parts.commentOnly = false;
		} else if (regex_search(s, s3, m3)) {
			parts.opCode = s3[1];
			parts.commentOnly = false;
		}

		return parts;
	}
	//write statement in list file
	void writeIntermediateFile(statParts parts, string error) {
		this->lineNumber++;
		fstream listFile;
		listFile.open("listFile.txt", ios_base::app);
		if (listFile.is_open()) {
			listFile << lineNumber;
			for (int i = 0; i < 6 - numOfDigit(lineNumber); i++)
				listFile << " ";

			listFile << hex << locCtr;
			for (unsigned i = 0; i < 10 - int_to_hex(locCtr).size(); i++)
				listFile << " ";

			listFile << parts.label;
			for (unsigned i = 0; i < 10 - parts.label.size(); i++)
				listFile << " ";

			listFile << parts.opCode;
			for (unsigned i = 0; i < 10 - parts.opCode.size(); i++)
				listFile << " ";

			listFile << parts.operand;
			for (unsigned i = 0; i < 10 - parts.operand.size(); i++)
				listFile << " ";
			listFile << parts.comment << endl;

			if (!error.empty())
				listFile << "                    " << error << endl;
			listFile.close();
		}
	}
	//function to convert int to hex
	string int_to_hex(int i) {

		stringstream stream;
		stream << std::hex << i;
		return stream.str();
	}
	//function to convert  hex to int
	int hex_to_int(string hex) {
		int size = hex.size();
		int num = 0;
		for (int i = 0; i < size; i++) {
			char c = hex.at(size - 1 - i);
			if (c >= '0' && c <= '9') {
				num += (c - '0') * pow(16, i);
			} else if (c >= 'A' && c <= 'F') {
				num += ((c - 'A') + 10) * pow(16, i);
			} else if (c >= 'a' && c <= 'f') {
				num += ((c - 'a') + 10) * pow(16, i);
			} else
				return 0;
		}
		return num;
	}
	//to determine number of digits in number
	int numOfDigit(int i) {
		int c = 0;
		while (i != 0) {
			c++;
			i = i / 10;

		}
		return c;
	}
	bool isInt(string s) {
		for (unsigned i = 0; i < s.size(); i++) {
			if (s.at(i) < '0' || s.at(i) > '9')
				return false;
		}
		return true;
	}
	void writeSmybolTable() {
		if (symTable.size() != 0) {
			fstream listFile;
			listFile.open("listFile.txt", ios_base::app);
			if (listFile.is_open()) {
				listFile << endl << endl;
				listFile << "----------------------------" << endl;
				listFile << "        Symbol Table        " << endl;
				listFile << "----------------------------" << endl;
				listFile << "    label    |   address   |" << endl;
				for (const pair<const string, labelInfo> &p : symTable) {
					listFile << " " << p.first;
					for (unsigned i = 0; i < 12 - p.first.size(); i++)
						listFile << " ";
					listFile << "|";
					listFile << " " << p.second.address;
					for (unsigned i = 0; i < 12 - p.second.address.size(); i++)
						listFile << " ";
					listFile << "|" << endl;

				}
				listFile << "--------------------------------" << endl;
				listFile.close();
			}
		}
	}
public:
	// @suppress("Class members should be properly initialized")
	Pass1() {
		this->lenghtOfProgram = 0;
		this->lineNumber = 0;
		this->locCtr = 0;
		this->startingAddress = 0;
		this->noError = true ;
		fstream listFile;
		listFile.open("listFile.txt", ios_base::out);
		if (listFile.is_open()) {
			listFile << "line  Address   label     opcode    operand   comment" << endl;
			listFile.close();
		}
		constructOpTable();
	}
	//get operation table
	const map<string, opCodeInfo>& getOpCodeTable() const {
		return opCodeTable;
	}
	//set operation table if needed
	void setOpCodeTable(const map<string, opCodeInfo>& opCodeTable) {
		this->opCodeTable = opCodeTable;
	}
	//get symbol table after excution
	const map<string, labelInfo>& getSymTable() const {
		return symTable;
	}
	//perfom opertions in pass 1 (assign addresses to statements and labels and indicate errors)
	void excutePass1(string pathSourceFile) {
		ifstream sourceFile;
		sourceFile.open(pathSourceFile, ios_base::in);
		if (sourceFile.is_open()) {
			string line;
			bool flag = true;
			getline(sourceFile, line);
			statParts parts = parseLine(line);
			//if there are some comment lines before start statement
			while (parts.commentOnly) {
				writeIntermediateFile(parts, "");
				if (!sourceFile.good()) { //check if file doesn't contain other statements
					flag = false;
					break;
				}
				getline(sourceFile, line);
				parts = parseLine(line);
			}
			//check if there is some line after first comment lines
			if (flag) {
				string error;
				bool flagStart = true;
				string opCode = parts.opCode;
				transform(opCode.begin(), opCode.end(), opCode.begin(),
						::toupper);
				//if first statement is start statement with specified operand
				if (opCode.compare("START") == 0) {
//					try {
					if (!parts.operand.empty()) {
						startingAddress = hex_to_int(parts.operand);
//						if (startingAddress == 0)
//							error = "invalid operand in starting address";
					}

					writeIntermediateFile(parts, error);

				} else {
					error =
							"warning : first statement is prefered to  include start directive";
					flagStart = false;
				}
				if (sourceFile.good()) {
					if (flagStart)
						getline(sourceFile, line);
					locCtr = startingAddress;
					parts = parseLine(line);
					opCode = parts.opCode;
					transform(opCode.begin(), opCode.end(), opCode.begin(),
							::toupper);

					bool endFlag = true;
					int prevLoct = 0;
					bool flagOrg = false;
					//excute statements untill end is countered
					while (opCode.compare("END") != 0) {
						int increasLoc = 0;

						error = "";
						if (!parts.commentOnly) {
							//if symbol was found
							if (!parts.label.empty()) {
								auto pairFound = symTable.find(parts.label);
								if (pairFound != symTable.end()) {
									pairFound->second.flag = true;
									noError=false;
									error += "    error : symbol  "
											+ parts.label + "already defined";
								} else {
									labelInfo info;
									info.address = int_to_hex(locCtr);
									info.flag = false;
									symTable[parts.label] = info;
								}
							}
							//if operation code exist
							if (!parts.opCode.empty()) {
								bool format4 = (parts.opCode.at(0) == '+');

								if (format4)
									opCode.replace(0, 1, "");

								auto pairFound = opCodeTable.find(opCode);
								if (pairFound != opCodeTable.end()) {
									//check format of instruction
									if (pairFound->second.format == 3) {
										if (format4)
											increasLoc = 4;
										else
											increasLoc = 3;
									} else if (pairFound->second.format == 2) {
										if (format4){
                                            error +=
													"  error  : + is not defined for format 2     ";
													noError=false;
										}

										else
											increasLoc = 2;
									} else if (pairFound->second.format == 1) {
										if (format4)
											{
											    error +=
													"  error : + is not defined for format 1";
                                                noError = false;
											}
										else
											increasLoc = 1;
									}
									//check number of operands
									regex mat(
											"\\s*[abstfxl]\\s*,\\s*[abstfxl]\\s*",
											regex_constants::icase);
									if (pairFound->second.numOfOperand == 1
											&& regex_match(parts.operand, mat))
										{
										    error +=
												"  error : not valid operand ,one operand is allowed  ";
                                            noError = false;
										}
									else if (pairFound->second.numOfOperand == 2
											&& !regex_match(parts.operand, mat)){

											error +=
												"  error : this opCode need two operand  ";
												noError=false;
											}


									else if (pairFound->second.numOfOperand == 0
											&& !parts.operand.empty())
										error +=
												"   warning : this opCode doesn't require operand";
								} else if (opCode.compare("BYTE") == 0) {
									regex eX("\\s*x'[abcdef[:d:]]+'\\s*",
											regex_constants::icase);
									regex eC("\\s*c'.+\\s*'",
											regex_constants::icase);
									if (regex_match(parts.operand, eX)) {

										increasLoc = ceil(
												(parts.operand.length() - 3)
														/ 2);
									} else if (regex_match(parts.operand, eC)) {
										increasLoc = parts.operand.length() - 3;
									} else {
                                                    noError=false;
										error +=
												"   error : not valid operand , operand must be hexdecimal or caracter";
									}
								} else if (opCode.compare("WORD") == 0) {
									try {
										stoi(parts.operand);
										increasLoc = 3;
									} catch (invalid_argument const &e) {
									    noError=false;
										error +=
												"   error : not valid operand,operand must be integer";
									} catch (out_of_range const &e) {
									    noError=false;
										error +=
												"   error:  not valid operand,operand must be integer";
									}
								} else if (opCode.compare("RESB") == 0) {
									int bytes = 0;
									try {
										bytes = stoi(parts.operand);
										increasLoc = bytes;
									} catch (invalid_argument const &e) {
									    noError=false;
										error +=
												"   error : not valid operand, ,operand must be integer";
									} catch (out_of_range const &e) {
									    noError=false;
										error +=
												"   error : not valid operand , operand must be integer";
									}
								} else if (opCode.compare("RESW") == 0) {
									int words = 0;
									try {
										words = stoi(parts.operand);
										increasLoc = words * 3;
									} catch (invalid_argument const &e) {
									     noError=false;
										error +=
												"   error : not valid operand, operand must be integer";
									} catch (out_of_range const &e) {
									     noError=false;
										error +=
												"   error : not valid operand, operand must be integer";
									}
								} else if (opCode.compare("EQU")==0) {
									if (!parts.label.empty()
											&& !parts.operand.empty()) {
										if (isInt(parts.operand)) {
											labelInfo l;
											l.address = parts.operand;
											l.flag = false;
											symTable[parts.label] = l;
										} else {
											auto pairFound = symTable.find(
													parts.operand);
											if (pairFound != symTable.end()) {
												symTable[parts.label] =
														pairFound->second;
											} else
												error +=
														" warning : define constant failed,operand must be defined  previously ";
										}
									} else if (!parts.label.empty())
										error +=
												" warning : EQU statement requires operand feild";
									else if (!parts.operand.empty())
										error +=
												" warning : EQU statement requires label feild";
									else
										error +=
												" warning : EQU statement requires operand and label feild";
								} else if (opCode.compare("ORG")==0) {

									if (!flagOrg) {
										flagOrg = true;
										if (!parts.operand.empty()) {
											auto pairFound = symTable.find(
													parts.operand);
											if (pairFound != symTable.end()) {
												prevLoct = locCtr;
												locCtr =
														hex_to_int(
																pairFound->second.address);
											} else
												error +=
														" warning : assigning location counter failed,operand must be defined  previously ";
										}
									} else{
										locCtr = prevLoct;
										flagOrg = false;
									}


								} else if (opCode.compare("BASE") != 0
										&& opCode.compare("NOBASE") != 0) {
										     noError=false;
									error +=
											"   error : not valid operation code";
								}
							}

						}
						writeIntermediateFile(parts, error);
						locCtr += increasLoc;
						if (sourceFile.good()) {
							getline(sourceFile, line);
							parts = parseLine(line);
							opCode = parts.opCode;
							transform(opCode.begin(), opCode.end(),
									opCode.begin(), ::toupper);
						} else {
							endFlag = false;
							break;
						}

					}
					if (!endFlag) {
						error =
								"warning : last line of program must determine with end statement";
						parts.comment = "";
						parts.opCode = "";
						parts.operand = "";
						parts.label = "";
					}
					writeIntermediateFile(parts, error);
					writeSmybolTable();
				}
			}
		} else
			cout << "source file path is not valid" << endl;
	}
	//operation table of sic/xe machine
	void constructOpTable() {
		opCodeInfo opInfo;
		//ADD statement
		opInfo.code = "18";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["ADD"] = opInfo;
		//ADDF statement
		opInfo.code = "58";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["ADDF"] = opInfo;
		//ADDR statement
		opInfo.code = "90";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["ADDR"] = opInfo;
		//AND statement
		opInfo.code = "40";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["AND"] = opInfo;
		//clear statement
		opInfo.code = "B4";
		opInfo.format = 2;
		opInfo.numOfOperand = 1;
		opCodeTable["CLEAR"] = opInfo;
		//COMP STATEMENT
		opInfo.code = "28";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["COMP"] = opInfo;
		//COMPF STATEMENT
		opInfo.code = "88";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["COMPF"] = opInfo;
		//COMPR STATEMENT
		opInfo.code = "A0";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["COMPR"] = opInfo;
		//DIV STATEMENT
		opInfo.code = "24";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["DIV"] = opInfo;
		//DIVF STATEMENT
		opInfo.code = "64";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["DIVF"] = opInfo;
		//DIVR STATEMENT
		opInfo.code = "9C";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["DIVR"] = opInfo;
		//FIX STATEMENT
		opInfo.code = "C4";
		opInfo.format = 1;
		opInfo.numOfOperand = 0;
		opCodeTable["FIX"] = opInfo;
		//FLOAT STATEMENT
		opInfo.code = "C0";
		opInfo.format = 1;
		opInfo.numOfOperand = 0;
		opCodeTable["FLOAT"] = opInfo;
		//HIO STATEMENT
		opInfo.code = "F4";
		opInfo.format = 1;
		opInfo.numOfOperand = 0;
		opCodeTable["HIO"] = opInfo;
		//J STATEMENT
		opInfo.code = "3C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["J"] = opInfo;
		//JEQ STATEMENT
		opInfo.code = "30";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["JEQ"] = opInfo;
		//JGT STATEMENT
		opInfo.code = "34";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["JGT"] = opInfo;
		//JTL STATEMENT
		opInfo.code = "38";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["JLT"] = opInfo;
		//JSUB STATEMENT
		opInfo.code = "48";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["JSUB"] = opInfo;
		//LDA STATEMENT
		opInfo.code = "00";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDA"] = opInfo;
		//LDB STATEMENT
		opInfo.code = "68";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDB"] = opInfo;
		//LDCH STATEMENT
		opInfo.code = "50";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDCH"] = opInfo;
		//LDF STATEMENT
		opInfo.code = "70";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDF"] = opInfo;
		//LDL STATEMENT
		opInfo.code = "08";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDL"] = opInfo;
		//LDS STATEMENT
		opInfo.code = "6C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDS"] = opInfo;
		//LDT STATEMENT
		opInfo.code = "74";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDT"] = opInfo;
		//LDX STATEMENT
		opInfo.code = "04";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LDX"] = opInfo;
		//LPS STATEMENT
		opInfo.code = "D0";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["LPS"] = opInfo;
		//MUL STATEMENT
		opInfo.code = "20";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["MUL"] = opInfo;
		//MULF STATEMENT
		opInfo.code = "60";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["MULF"] = opInfo;
		//MULR STATEMENT
		opInfo.code = "98";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["MULR"] = opInfo;
		//NORM STATEMENT
		opInfo.code = "C8";
		opInfo.format = 1;
		opInfo.numOfOperand = 0;
		opCodeTable["NORM"] = opInfo;
		//OR STATEMENT
		opInfo.code = "44";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["OR"] = opInfo;
		//RD STATEMENT
		opInfo.code = "D8";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["RD"] = opInfo;
		//RMO STATEMENT
		opInfo.code = "AC";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["RMO"] = opInfo;
		//RSUB STATEMENT
		opInfo.code = "4C";
		opInfo.format = 3;
		opInfo.numOfOperand = 0;
		opCodeTable["RSUB"] = opInfo;
		//SHIFTL STATEMENT
		opInfo.code = "A4";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["SHIFTL"] = opInfo;
		//SHIFTR STATEMENT
		opInfo.code = "A8";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["SHIFTR"] = opInfo;
		//SIO STATEMENT
		opInfo.code = "F0";
		opInfo.format = 1;
		opInfo.numOfOperand = 0;
		opCodeTable["SIO"] = opInfo;
		//SSK STATEMENT
		opInfo.code = "EC";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["SSK"] = opInfo;
		//STA STATEMENT
		opInfo.code = "0C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STA"] = opInfo;
		//STB STATEMENT
		opInfo.code = "78";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STB"] = opInfo;
		//STCH STATEMENT
		opInfo.code = "54";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STCH"] = opInfo;
		//STF STATEMENT
		opInfo.code = "80";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STF"] = opInfo;
		//STI STATEMENT
		opInfo.code = "D4";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STI"] = opInfo;
		//STL STATEMENT
		opInfo.code = "14";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STL"] = opInfo;
		//STS STATEMENT
		opInfo.code = "7C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STS"] = opInfo;
		//STSW STATEMENT
		opInfo.code = "E8";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STSW"] = opInfo;
		//STT STATEMENT
		opInfo.code = "84";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STT"] = opInfo;
		//STX STATEMENT
		opInfo.code = "10";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["STX"] = opInfo;
		//SUB STATEMENT
		opInfo.code = "1C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["SUB"] = opInfo;
		//SUBF STATEMENT
		opInfo.code = "5C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["SUBF"] = opInfo;
		//SUBR STATEMENT
		opInfo.code = "94";
		opInfo.format = 2;
		opInfo.numOfOperand = 2;
		opCodeTable["SUBR"] = opInfo;
		//SVC STATEMENT
		opInfo.code = "B0";
		opInfo.format = 2;
		opInfo.numOfOperand = 1;
		opCodeTable["SVC"] = opInfo;
		//TD STATEMENT
		opInfo.code = "E0";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["TD"] = opInfo;
		//TIO STATEMENT
		opInfo.code = "F8";
		opInfo.format = 1;
		opInfo.numOfOperand = 0;
		opCodeTable["TIO"] = opInfo;
		//TIX STATEMENT
		opInfo.code = "2C";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["TIX"] = opInfo;
		//TIXR STATEMENT
		opInfo.code = "B8";
		opInfo.format = 2;
		opInfo.numOfOperand = 1;
		opCodeTable["TIXR"] = opInfo;
		//WD STATEMENT
		opInfo.code = "DC";
		opInfo.format = 3;
		opInfo.numOfOperand = 1;
		opCodeTable["WD"] = opInfo;
	}

}
;

#endif /* PARSER_H_ */
