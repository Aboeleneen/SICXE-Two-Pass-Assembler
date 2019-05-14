#ifndef PASS2_H_INCLUDED
#define PASS2_H_INCLUDED

#include<bits/stdc++.h>
#include "parser.h"
#include "numberBases.h"
#include<iostream>

using namespace std;
struct expResult
{
    string value = "";
    bool isRelative = false;
};

class pass2
{
public:
    string locationCounter;
    string base;
    string startingAddress;
    string programName ;
    string originCounter ;
    int lineNumber ;
    bool passOneDone;
    map<string, opCodeInfo> opCodeTable;
    map<string, labelInfo> symTable;
    map<string,string> registers ;
    vector< pair<string,string> > objectCodes ;
    Pass1 pass1;
    ifstream inFile  ;
    ofstream outFile ;
    ofstream objectFile;
    void defineRegisters()
    {
        registers["A"]="0";
        registers["X"]="1";
        registers["L"]="2";
        registers["B"]="3";
        registers["S"]="4";
        registers["T"]="5";
        registers["F"]="6";
        registers["PC"]="7";
        registers["SW"]="8";
    }

    void excutePassOne(string fileName)
    {
        pass1.excutePass1(fileName);
        passOneDone=pass1.noError;
    }

    bool isComment(string str)
    {
        regex comment("\\.[^]+");
        if(regex_search(str,comment))
        {
            return true ;
        }
        else
        {
            return false;
        }
    }

    bool isError(string str)
    {
        regex error("error[^]+");
        if(regex_search(str,error))
        {
            return true ;
        }
        else
        {
            return false;
        }
    }

    bool isExpress(string s)
    {
        regex e("([a-z0-9]+)([*|+|/|\-])([a-z0-9])");
        if(regex_match(s,e))
        {
            return true ;
        }
        else
        {
            return false;
        }
    }

    // get location counter of current instruction
    string getPC(string line)
    {
        regex e("^([0-9]+[ ]+)",regex_constants::icase);
        line=regex_replace(line,e,"");
        regex pc("^([0-9a-f]+)",regex_constants::icase);
        smatch m ;
        regex_search(line,m,pc);
        locationCounter=upperCase(m[1]);
        line=regex_replace(line,pc,"");
        return line;
    }
    // calculate object code for each instruction
    string getObjectCode(statParts stat, bool format4)
    {
        //  cout << stat.operand << endl;
        int format = opCodeTable[stat.opCode].format;
        if(format4)
            format=4;
        // RSUB
        if(opCodeTable[stat.opCode].numOfOperand==0)
        {
            return  convertBinToHex(getOpcodeBin(opCodeTable[stat.opCode].code,6)+"110000")+"000";
        }
        // check that operand is an expression
        bool isExpression = false;
        if(isExpress(stat.operand)){
                cout << stat.operand <<  " done " << endl;
                expResult operand = calExpression(stat.operand);
                isExpression = true;
                if(!operand.isRelative){
                    stat.operand = '#' + operand.value;
                }else{
                    stat.operand = operand.value;
                }
        }
        if(format==2)
        {
            locationCounter = addHex(locationCounter,"02");
            opCodeInfo opcode = opCodeTable[stat.opCode];
            string firstRegister  =  stat.operand.substr(0,1);
            if(stat.operand.length()!=1)
            {
                string secondRegister = stat.operand.substr(2,3);
                return opcode.code + registers[firstRegister] +registers[secondRegister];
            }
            else
            {
                return opcode.code + registers[firstRegister] +"0";
            }

        }

        string objectCode = getStringWithLenght(8*format);
        opCodeInfo opcode = opCodeTable[stat.opCode];
        string opcodeBin = getOpcodeBin(opcode.code,6);
        string nixbpe ="000000" ;

        // immediate address
        if(stat.operand.at(0)=='#')
        {
            if(!isExpression)
                nixbpe[1]='1';
            stat.operand = stat.operand.substr(1,stat.operand.length());
            if(symTable[stat.operand].address==""||isExpression)
            {
                if(!isExpression)
                    stat.operand = convertFromBase10(convertStringToInt(stat.operand),16);
                string TA = convertHexToBin(stat.operand);
                if(format==4)
                {
                    nixbpe[5]=1;
                    while(TA.length()<20)
                        TA = "0"+TA;
                }
                else
                {
                    while(TA.length()<12)
                        TA = "0"+TA;
                }
                objectCode = opcodeBin + nixbpe + TA;

                return convertBinToHex(objectCode);
            }
        }
        // indirect address
        else if(stat.operand.at(0)=='@')
        {
            nixbpe[0]='1';
            stat.operand = stat.operand.substr(1,stat.operand.length());
        }
        else
        {
            nixbpe[0]='1';
            nixbpe[1]='1';
        }
        // indexing address
        regex e(",X");
        if(regex_search(stat.operand,e))
        {
            nixbpe[2]='1';
            stat.operand = regex_replace(stat.operand,e,"");
        }
        if(format==3)
        {
            locationCounter = addHex(locationCounter,"03");
            string TA = "";
            if(isExpression)
            {
                TA = upperCase(stat.operand);
            }
            else
            {
                TA = upperCase(symTable[stat.operand].address);
            }
            string displacementPC = subHex(TA,locationCounter,3);
            string displacementB = subHex(TA,base,3);
            string displacement="";

            long long disp1 = convertToBase10(TA,16) - convertToBase10(locationCounter,16);
            long long disp2 = convertToBase10(TA,16) - convertToBase10(base,16);
            if(disp1>=-2048&&disp1<=2047)
            {
                nixbpe[4]='1';
                displacement=displacementPC;
            }
            else if(disp2>=0&&disp2<=4095)
            {
                nixbpe[3]='1';
                displacement=displacementB;
            }
            else
            {
                return "overflow error" ;
            }
            displacement=convertHexToBin(displacement);
            while(displacement.length()<12)
            {
                displacement="0"+displacement;
            }
            objectCode = opcodeBin + nixbpe + displacement;
            return convertBinToHex(objectCode) ;

        }
        else if (format==4)
        {
            locationCounter = addHex(locationCounter,"04");
            nixbpe[5]='1';
            string TA = "";
            if(isExpression)
            {
                TA = upperCase(stat.operand);
            }
            else
            {
                TA = upperCase(symTable[stat.operand].address);
            }
            TA=convertHexToBin(TA);
            while(TA.length()<20)
                TA = "0"+TA;
            objectCode = opcodeBin + nixbpe + TA ;
            return convertBinToHex(objectCode);
        }


    }

    expResult calExpression(string exp)
    {
        regex e("([a-zA-Z0-9]+)([*|+|/|\-])([a-zA-Z0-9]+)");
        smatch m ;
        string first, second, oper ;
        bool relativeFirst, relativeSecond;
        relativeFirst=relativeSecond=false;
        expResult res ;
        res.value="ERROR";
        if(regex_search(exp,m,e))
        {
            first  = m[1];
            oper   = m[2];
            second = m[3];
        }
        if(symTable[first].address!="")
        {
            first = symTable[first].address;
            relativeFirst=symTable[second].flag;
        }
        else if(!checkHex(first))
        {
            return res ;
        }
        if(symTable[second].address!="")
        {
            second = symTable[second].address;
            relativeSecond=symTable[second].flag;
        }
        else if(!checkHex(second))
        {
            return res ;
        }

        if(oper.at(0)=='+')
        {
            if(relativeFirst&&relativeSecond)
            {
                return res ;
            }
            else if(relativeFirst||relativeSecond)
            {
                res.isRelative=true;
            }
            res.value=addHex(first,second);
            return res;
        }

        if(oper.at(0)=='-')
        {
            if(!relativeFirst&&relativeSecond)
            {
                return res;
            }
            else if(relativeFirst||relativeSecond)
            {
                res.isRelative=true;
            }
            res.value=subHex(first,second,3);
            return res;
        }

        if(oper.at(0)=='*')
        {
            if(relativeFirst||relativeSecond)
            {
                return res;
            }
            res.value=mulHex(first,second);
            return res;
        }

        if(oper.at(0)=='/')
        {
            if(relativeFirst||relativeSecond)
            {
                return res;
            }
            res.value=divHex(first,second);
            return res;
        }

    }

    void writeListFile(statParts line,string address,string objectcode)
    {
        outFile.open("listFile.txt",ios_base::app);
        this->lineNumber++;
        // line number
        int num = pass1.numOfDigit(lineNumber);
        outFile << lineNumber ;
        for(int i=num; i<14; i++)
            outFile << " ";
        // address
        outFile << address;
        num=address.length();
        for(int i=num; i<13; i++)
            outFile << " ";
        // label
        outFile << line.label ;
        num=line.label.length();
        for(int i=num; i<13; i++)
            outFile << " ";
        // opcode
        outFile << line.opCode ;
        num=line.opCode.length();
        for(int i=num; i<13; i++)
            outFile << " ";
        // operands
        outFile << line.operand;
        num=line.operand.length();
        for(int i=num; i<16; i++)
            outFile << " ";
        // objectCode
        outFile << objectcode;
        outFile << "\n";
        outFile.close();
    }

    void writeError(string error)
    {
        outFile.open("listFile.txt",ios_base::app);
        outFile << "**Error:  "  << error  << "\n";
        outFile.close();
    }

    void writeHeaderRecord()
    {
        objectFile.open("objectFile.txt",ios_base::trunc);
        while(programName.length()<7)
            programName+=" ";
        while(startingAddress.length()<6)
            startingAddress  = "0" + startingAddress;
        objectFile << "H " << this->programName << "  " << this->startingAddress << " " <<  subHex(locationCounter,startingAddress,2) << "\n";
        objectFile.close();
    }

    void writeTextRecord()
    {
        objectFile.open("objectFile.txt",ios_base::app);
        for(int i=0; i<objectCodes.size();)
        {
            string start = objectCodes.at(i).first;
            string finish = locationCounter ;
            int j = 1 ;
            vector<string> textRecord ;
            textRecord.push_back(objectCodes.at(i).second);
            i++;
            while(j<5&&i<objectCodes.size())
            {
                textRecord.push_back(objectCodes.at(i).second);
                finish = objectCodes.at(i).first;
                j++;
                i++;
            }
            while(start.length()<6)
                start = "0"+start;
            objectFile << "T" << " " << start << " " << subHex(finish,start,2) << " " ;
            for(j=0; j<textRecord.size(); j++)
            {
                string text = textRecord.at(j);
                while(text.length()<6)
                    text = "0"+text;
                objectFile << text << " ";
            }
            objectFile << "\n";
        }
        objectFile << "E " << startingAddress << "\n" ;
        objectFile.close();
    }


public:
    pass2(string fileName)
    {
        this->locationCounter="";
        this->base="0000";
        this->passOneDone=false;
        this->lineNumber=0;
        this->originCounter="";
        excutePassOne(fileName);
        defineRegisters();
        opCodeTable=pass1.getOpCodeTable();
        symTable=pass1.getSymTable();
        startingAddress=pass1.startingAddress;
        inFile.open("listFile.txt", ios_base::in);
        outFile.open("listFile.txt",ios_base::app);
        outFile << "\n***********************************************\n\n START PASS TWO \n\n" ;
        outFile << "LineNumber    ADDRESS      LABEL       Mnemonic     OPERANDS        OBJECTCODE\n";
        outFile.close();
    }

    void excutePassTwo()
    {
        if(inFile.is_open())
        {
            string line ;

            getline(inFile,line);
            getline(inFile,line);
            // ignore all lines before start instruction
            while(isComment(line))
            {
                getline(inFile,line);
            }
            line = getPC(line);
            //cout << line << endl;
            statParts stat = pass1.parseLine(line);
            stat.opCode = upperCase(stat.opCode);
            if(stat.opCode.compare("START")==0)
            {
                programName=stat.label;
                startingAddress = (stat.operand==""?0:stat.operand);
            }

            // write header record here
            while(stat.opCode.compare("END")!=0)
            {
                getline(inFile,line);
                line = getPC(line);
                string currentLC = locationCounter ;
                stat = pass1.parseLine(line);

                // check if this line is a comment
                if(isComment(line))
                {
                    continue;
                }
                bool format4=false;
                if(stat.opCode.at(0)=='+')
                {
                    format4=true;
                    stat.opCode=stat.opCode.substr(1,stat.opCode.length());
                }
                stat.opCode = upperCase(stat.opCode);

                // check if opcode table contain this opcode or not
                if(opCodeTable[stat.opCode].code!="")
                {
                    // get object for this line
                    string objectCode = this->getObjectCode(stat,format4);
                    objectCodes.push_back(make_pair(currentLC,objectCode));
                    if(objectCode=="overflow error")
                    {
                        this->writeError(objectCode);
                        continue;
                    }
                    this->writeListFile(stat,currentLC,objectCode);
                    continue;
                }
                // change base
                else if(stat.opCode=="BASE")
                {
                    base = symTable[stat.operand].address;
                }
                //BYTE OR WORD --> OBJECT CODE EQUAL OPERAND
                else if(stat.opCode=="BYTE"||stat.opCode=="WORD")
                {

                    string operand = stat.operand;
                    // X'HEXA NUMBER'
                    if(stat.operand.at(0)=='X')
                    {
                        string s ="";
                        for(int i=2; i<stat.operand.length()-1; i++)
                            s+=stat.operand.at(i);
                        operand=s;
                    }
                    else if (stat.operand.at(0)=='C')
                    {
                        string s ="";
                        for(int i=2; i<stat.operand.length()-1; i++)
                        {
                            s+=(convertFromBase10((long long)stat.operand.at(i),16));
                        }
                        operand=s;
                    }
                    objectCodes.push_back(make_pair(locationCounter,operand));
                    this->writeListFile(stat,locationCounter,operand);
                    continue;
                }
                // RESTORE LOCATION COUNTER
                else if (stat.opCode=="ORG")
                {
                    if(stat.operand!="")
                    {
                        originCounter=locationCounter;
                        locationCounter=stat.operand;
                    }
                    else
                    {
                        locationCounter=originCounter;
                    }
                    continue;
                }

                this->writeListFile(stat,locationCounter,"");


            }
            this->writeHeaderRecord();
            this->writeTextRecord();
        }
    }

};

#endif // PASS2_H_INCLUDED
