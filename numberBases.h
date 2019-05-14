#ifndef NUMBERBASES_H_INCLUDED
#define NUMBERBASES_H_INCLUDED

#include<bits/stdc++.h>

using namespace std;
string subHex(string num1, string num2, int length);

string letters = "0123456789ABCDEF" ;
int toInt(char c)
{
    return letters.find(c) ;
}
long long convertToBase10(string num, long long base)
{
    long long res = 0 ;
    for(int i=0; i<num.size(); i++)
    {
        res*=base ;
        res+=toInt(num[i]);
    }
    return res ;
}
string convertFromBase10(long long num, long long base)
{
    string res="" ;
    while(num!=0)
    {
        int lastDigit = num%base;
        res=letters[lastDigit]+res;
        num/=base;
    }
    return res ;
}
// convert hexadecimal number to binary
string convertHexToBin(string num)
{
    long long n = convertToBase10(num,16);
    return convertFromBase10(n,2);
}

// convert binary number to hexadecimal
string convertBinToHex(string num)
{
    long long n = convertToBase10(num,2);
    return convertFromBase10(n,16);
}
// calculate the result of adding two hex digits
string addHex(string num1, string num2)
{
    long long n1 = convertToBase10(num1,16);
    long long n2 = convertToBase10(num2,16);
    return convertFromBase10(n1+n2,16);
}
// calculate twos complement
string twosComplement(string num)
{
    string n ;
    while(n.length()!=num.length())
        n+='F';
    num = subHex(n,num,0);
    return addHex(num,"1");
}

// calculate the result of subtraction two hex digits
string subHex(string num1, string num2, int length)
{
    long long n1 = convertToBase10(num1,16);
    long long n2 = convertToBase10(num2,16);
    if(n1-n2>=0)
        return convertFromBase10(n1-n2,16);
    string res = convertFromBase10(n2-n1,16);
    while(res.length()<(length))
        res = "0" + res ;
    return twosComplement(res);
}

// multiply two hex numbers
string mulHex(string num1,string num2)
{
    long long n1 = convertToBase10(num1,16);
    long long n2 = convertToBase10(num2,16);
    return convertFromBase10(n1*n2,16);
}

// division two hex numbers
string divHex(string num1,string num2)
{
    long long n1 = convertToBase10(num1,16);
    long long n2 = convertToBase10(num2,16);
    return convertFromBase10(n1/n2,16);
}
// to get a string with specific length
string getStringWithLenght(int length)
{
    string s ;
    for(int i=0; i<length; i++)
        s+="0";
    return s ;
}

// calculate opCode in binary to specific format
string getOpcodeBin(string opcode, int numOfBits)
{

    long long opcodeInt = convertToBase10(opcode,16);
    string opcodeBin = convertFromBase10(opcodeInt,2);
    while(opcodeBin.length()<8)
        opcodeBin="0"+opcodeBin;
    opcodeBin=opcodeBin.substr(0,numOfBits);
    return opcodeBin;
}

string convertIntToString(long long num)
{
    stringstream convert ;
    convert << num ;
    return convert.str();
}

long long convertStringToInt(string num)
{
    stringstream convert(num) ;
    long long num2=0;
    convert >> num2 ;
    return num2;
}
// convert string to upper case
string upperCase(string num)
{
    for(int i=0; i<num.length(); i++)
    {
        if(num[i]>='a'&&num[i]<='z')
        {
            num[i]-=32;
        }
    }
    return num;
}
// check that number is a hexadecimal number
bool checkHex(string num)
{
    num = upperCase(num);
    for(int i=0; i<num.length(); i++)
    {
        if(num[i]>='0'&&num[i]<='9')
            continue;
        if(num[i]>='A'&&num[i]<='F')
            continue;
        return false;
    }
    return true;
}


#endif // NUMBERBASES_H_INCLUDED
