
/*
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include<sstream>
#include <fstream>
#include<time.h>
using namespace std;





class map_value_finder
{
public:
	map_value_finder(const std::string &cmp_string) :m_s_cmp_string(cmp_string){}
	bool operator ()(const std::map<int, std::string>::value_type &pair)
	{
		return pair.second == m_s_cmp_string;
	}
private:
	const std::string &m_s_cmp_string;
};


class myMap
{
public:
	void initMap()
	{
		cnt = 0;
		for (int i = 0; i <= 255; i++)
		{
			Dic.insert(std::pair<int, string>(i, "-1"));
			cnt++;
		}
	}
	void initCompressMap()
	{
		dDic.clear();
		cnt = 256;
	}
	void insertNew(const string &newcode)
	{
		if (newcode.length() < 2 && (int)newcode[0] <= 255)
		{
			Dic[(int)newcode[0]] = newcode;
		}
		else
		{
			Dic.insert(std::pair<int, string>(cnt, newcode));
			cnt++;
		}
	}
	void compressInsert(const string &newcode)
	{
		if (newcode.length() < 2 && (int)newcode[0] <= 255)
		{
			dDic.insert(std::pair<string, int>(newcode, (int)newcode[0]));
		}
		else
		{
			dDic.insert(std::pair<string, int>(newcode, cnt));
			cnt++;
		}
	}

	int getCode(const string &value)
	{
		return dDic[value];
	}
	int existKey(string str)
	{
		return dDic.count(str);
	}
	string getValue(const int &num)
	{
		return Dic.at(num);
	}
	int getSize(){
		return Dic.size();
	}
	int findByValue(string value)
	{
		std::map<int, string>::iterator it = Dic.end();
		it = std::find_if(Dic.begin(), Dic.end(), map_value_finder(value));
		if (it == Dic.end())
		{
			return -2;
		}
		else
			return it->first;
	}
	void goTo(std::ostream &s)
	{
		std::map<string, int>::iterator iter = dDic.begin();
		while (iter != dDic.end())
		{
			s << iter->first << ":" << std::hex << iter->second << std::endl;
			iter++;
		}
	}

private:
	std::map<string, int> dDic;
	std::map<int, string> Dic;
	int cnt = 0;
};











struct node
{
	string P;   //previous
	char C;	    //current
	string S;
};

class encrypt
{
public:
	encrypt()
	{
	}

	void Compress(char *inFile, char *chFile)
	{
		startTime = clock();
		initNd();
		mp.initCompressMap();

		std::ofstream out(chFile);
		std::ifstream in(inFile);
		while ((nd.C = in.get()) && nd.C != EOF)
		{
			std::stringstream stream;
			stream << nd.C;                   //字符转换为字符串
			mp.compressInsert(stream.str());          //把current读入的字符插入字典中
			searchDic(nd.C, out);          //搜索字典
		}

		out << std::hex << mp.getCode(nd.P) << " ";
		out.close();
		in.close();
		endTime = clock();
		std::cout << "压缩此文件耗时:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;    //程序运行时间的打印
	}
	void searchDic(char c, std::ofstream &s)
	{
		std::stringstream stream;
		stream << nd.P;
		stream << c;
		string str = stream.str();       //str=P+C
		if (mp.existKey(str) == 0)
		{
			s << std::hex << mp.getCode(nd.P) << " ";
			mp.compressInsert(str);
			nd.P = c;
		}
		else          //如果P在字典中,P=P+C
			nd.P = nd.P + c;
	}
	void Decompress(char *chFile, char *outFile)
	{
		startTime = clock();
		initNd();
		mp.initMap();
		int Q;
		std::ofstream out(outFile);
		std::ifstream in(chFile);
		in >> std::hex >> Q;      //以16进制读取至Q
		while (in.get() != EOF)
		{
			if (Q < mp.getSize() && mp.getValue(Q) == "-1")
			{
				//如果读入的字符属于ASCII表,强制转换成int并插入字典
				std::stringstream stream;
				stream << (char)Q;
				mp.insertNew(stream.str());
			}

			if (Q < mp.getSize())
			{
				//读入字符在字典里面,输出Q对应的值,在字典中添加tmp
				out << mp.getValue(Q);
				nd.S = mp.getValue(Q).substr(0, 1);         // S取Q对应的值的第一个字符
				string tmp = nd.P + nd.S;
				mp.insertNew(tmp);
			}
			else
			{
				//读入字符不在字典中,	tmp=P的值+P的值的第一位字符。 添加tmp的值,并输出tmp的值
				string tmp = nd.P + nd.P.substr(0, 1);
				mp.insertNew(tmp);
				out << tmp;
			}
			nd.P = mp.getValue(Q);
			in >> std::hex >> Q;
		}
		out.close();
		in.close();
		endTime = clock();
		std::cout << "解压此文件耗时:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;    //程序运行时间的打印
	}
	void printDic(char *inFile, char *chFile)
	{
		startTime = clock();
		initNd();
		mp.initCompressMap();
		std::ofstream out(chFile);
		std::ifstream in(inFile);
		while ((nd.C = in.get()) && nd.C != EOF)
		{
			std::stringstream stream;
			stream << nd.C;
			mp.compressInsert(stream.str());          // n
			searchDicNoPrint(nd.C);          //n^2
		}
		mp.goTo(out);   //字典遍历输出
		out.close();
		in.close();
		endTime = clock();
		std::cout << "打印字典耗时:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;    //程序运行时间的打印
	}
	void searchDicNoPrint(char c)
	{
		std::stringstream stream;
		stream << nd.P;
		stream << c;
		string str = stream.str();
		if (mp.existKey(str) == 0)
		{
			mp.compressInsert(str);
			nd.P = c;
		}
		else
			nd.P = nd.P + c;
	}

private:

	void initNd()
	{
		//清空字符串
		nd.P.clear();
		nd.S.clear();
	}

	node nd;
	myMap mp;
	clock_t startTime, endTime;
};









int main()
{
	char inFile[100], outFile[100], chFile[100];
	encrypt et;

	while (1)
	{
		int i;
		cout << "压缩文件输入1, 解压文件输入2, 打印字典输入3, 退出输入0:" << endl;
		cin >> i;
		try
		{
			if (cin.fail())
				throw "wrong!";
		}
		catch (const char*)
		{
			cin.clear();        //清楚错误输入的缓存
			cin.sync();
			cout << "请确认输入的为数字!" << endl;
			continue;
		}

		switch (i)
		{
		case 1:
		{
			cout << "输入被压缩文件地址:" << endl;
			cin >> inFile;
			cout << "输入压缩后文件地址:" << endl;
			cin >> chFile;
			et.Compress(inFile, chFile);
			break;
		}
		case 2:
		{
			cout << "输入被解压文件地址:" << endl;
			cin >> chFile;
			cout << "输入解压后文件地址:" << endl;
			cin >> outFile;
			et.Decompress(chFile, outFile);
			break;
		}
		case 3:
		{
			cout << "输入被压缩文件地址:" << endl;
			cin >> chFile;
			cout << "输入压缩后字典地址:" << endl;
			cin >> outFile;
			et.printDic(chFile, outFile);
			break;
		}
		case 0:
		{
			return 0;
		}
		default: cout << "无效的输入!" << endl;
		}
	}
	return 0;
}




*/

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include<sstream>
#include <fstream>
#include<time.h>
using namespace std;


class map_value_finder
{
public:
	map_value_finder(const std::string &cmp_string) :m_s_cmp_string(cmp_string){}
	bool operator ()(const std::map<int, std::string>::value_type &pair)
	{
		return pair.second == m_s_cmp_string;
	}
private:
	const std::string &m_s_cmp_string;
};


class myMap
{
public:
	void initMap()
	{
		cnt = 0;
		for (int i = 0; i <= 255; i++)
		{
			Dic.insert(std::pair<int, string>(i, "-1"));
			cnt++;
		}
	}
	void initCompressMap()
	{
		dDic.clear();
		cnt = 256;
	}
	void insertNew(const string &newcode)
	{
		if (newcode.length() < 2 && (int)newcode[0] <= 255)
		{
			Dic[(int)newcode[0]] = newcode;
		}
		else
		{
			Dic.insert(std::pair<int, string>(cnt, newcode));
			cnt++;
		}
	}
	void compressInsert(const string &newcode)
	{
		if (newcode.length() < 2 && (int)newcode[0] <= 255)
		{
			dDic.insert(std::pair<string, int>(newcode, (int)newcode[0]));
		}
		else
		{
			dDic.insert(std::pair<string, int>(newcode, cnt));
			cnt++;
		}
	}

	int getCode(const string &value)
	{
		return dDic[value];
	}
	int existKey(string str)
	{
		return dDic.count(str);
	}
	string getValue(const int &num)
	{
		return Dic.at(num);
	}
	int getSize(){
		return Dic.size();
	}
	int findByValue(string value)
	{
		std::map<int, string>::iterator it = Dic.end();
		it = std::find_if(Dic.begin(), Dic.end(), map_value_finder(value));
		if (it == Dic.end())
		{
			return -2;
		}
		else
			return it->first;
	}
	void goTo(std::ostream &s)
	{
		std::map<string, int>::iterator iter = dDic.begin();
		while (iter != dDic.end())
		{
			s << iter->first << ":" << std::hex << iter->second << std::endl;
			iter++;
		}
	}

private:
	std::map<string, int> dDic;
	std::map<int, string> Dic;
	int cnt = 0;
};



typedef struct _node
{
	string P;  
	char C;	    
	string S;
}MYNODE;

class encrypt
{
public:
	encrypt()
	{
	}

	void Compress(char *inFile, char *chFile)
	{
		startTime = clock();
		initNd();
		mp.initCompressMap();

		std::ofstream out(chFile);
		std::ifstream in(inFile);
		while ((nd.C = in.get()) && nd.C != EOF)
		{
			std::stringstream stream;
			stream << nd.C;                 
			mp.compressInsert(stream.str());         
			searchDic(nd.C, out);        
		}

		out << std::hex << mp.getCode(nd.P) << " ";
		out.close();
		in.close();
		endTime = clock();
		std::cout << "压缩此文件耗时:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;  
	}
	void searchDic(char c, std::ofstream &s)
	{
		std::stringstream stream;
		stream << nd.P;
		stream << c;
		string str = stream.str();    
		if (mp.existKey(str) == 0)
		{
			s << std::hex << mp.getCode(nd.P) << " ";
			mp.compressInsert(str);
			nd.P = c;
		}
		else      
			nd.P = nd.P + c;
	}
	void Decompress(char *chFile, char *outFile)
	{
		startTime = clock();
		initNd();
		mp.initMap();
		int Q;
		std::ofstream out(outFile);
		std::ifstream in(chFile);
		in >> std::hex >> Q;    
		while (in.get() != EOF)
		{
			if (Q < mp.getSize() && mp.getValue(Q) == "-1")
			{
				std::stringstream stream;
				stream << (char)Q;
				mp.insertNew(stream.str());
			}

			if (Q < mp.getSize())
			{
				out << mp.getValue(Q);
				nd.S = mp.getValue(Q).substr(0, 1);        
				string tmp = nd.P + nd.S;
				mp.insertNew(tmp);
			}
			else
			{
				string tmp = nd.P + nd.P.substr(0, 1);
				mp.insertNew(tmp);
				out << tmp;
			}
			nd.P = mp.getValue(Q);
			in >> std::hex >> Q;
		}
		out.close();
		in.close();
		endTime = clock();
		std::cout << "解压此文件耗时:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;   
	}
	void printDic(char *inFile, char *chFile)
	{
		startTime = clock();
		initNd();
		mp.initCompressMap();
		std::ofstream out(chFile);
		std::ifstream in(inFile);
		while ((nd.C = in.get()) && nd.C != EOF)
		{
			std::stringstream stream;
			stream << nd.C;
			mp.compressInsert(stream.str());        
			searchDicNoPrint(nd.C);        
		}
		mp.goTo(out);  
		out.close();
		in.close();
		endTime = clock();
		std::cout << "打印字典耗时:" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << std::endl;   
	}
	void searchDicNoPrint(char c)
	{
		std::stringstream stream;
		stream << nd.P;
		stream << c;
		string str = stream.str();
		if (mp.existKey(str) == 0)
		{
			mp.compressInsert(str);
			nd.P = c;
		}
		else
			nd.P = nd.P + c;
	}

private:

	void initNd()
	{
		//清空字符串
		nd.P.clear();
		nd.S.clear();
	}

	MYNODE nd;
	myMap mp;
	clock_t startTime, endTime;
};




int main()
{

	char inFile[100], outFile[100], chFile[100];
	encrypt et;

	while (1)
	{
		int i;
		cout << "压缩文件输入1, 解压文件输入2, 打印字典输入3, 退出输入0:" << endl;
		cin >> i;
		try
		{
			if (cin.fail())
				throw "wrong!";
		}
		catch (const char*)
		{
			cin.clear();        //清楚错误输入的缓存
			cin.sync();
			cout << "请确认输入的为数字!" << endl;
			continue;
		}

		switch (i)
		{
		case 1:
		{
			cout << "输入被压缩文件地址:" << endl;
			cin >> inFile;
			cout << "输入压缩后文件地址:" << endl;
			cin >> chFile;
			et.Compress(inFile, chFile);
			break;
		}
		case 2:
		{
			cout << "输入被解压文件地址:" << endl;
			cin >> chFile;
			cout << "输入解压后文件地址:" << endl;
			cin >> outFile;
			et.Decompress(chFile, outFile);
			break;
		}
		case 3:
		{
			cout << "输入被压缩文件地址:" << endl;
			cin >> chFile;
			cout << "输入压缩后字典地址:" << endl;
			cin >> outFile;
			et.printDic(chFile, outFile);
			break;
		}
		case 0:
		{
			return 0;
		}
		default: cout << "无效的输入!" << endl;
		}
	}
	return 0;
}


