#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	ifstream ifs("img.png", ios::binary);
	ofstream ofs;
	ofs.open("jt.png", ios::binary);
	if(!ifs.is_open()) cout << "NO\n";
	char buf[2000];
	while (1)
	{
		ifs.read(buf, 1999);
		if (!ifs.gcount())
		{
			cout << "NOoO\n";
			break;
		}
		buf[ifs.gcount()] = 0;
		std::string	s;// = buf;
		int i = 0;
		int ss = ifs.gcount();
		while (i < ss) 
			s.push_back(buf[i++]);
		std::string p = s;
		ofs.write(p.c_str(), p.size());
		// ofs << buf;
		//cout << buf << "\n";
	}
	ofs.close();
	ifs.close();
	return 0;
}