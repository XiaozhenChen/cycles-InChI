#include<iostream>
#include<vector>
#include<queue>
#include<string>
#include<unordered_set>


using namespace std;

struct InChI
{
	int version;
	int atom_count;
	string chemical;
	string atomLink;
	string hydragen;
};

vector<string> spliteInChI(string str);
InChI structInChI(string str);
vector<pair<string, int>> getAtomNumPair(string chemical);
int getNumber(const string &str, int &index);

/*
vector<pair<string, int>> getAtomNumPair(string chemical)
{
	vector<pair<string, int>> res;
	
	string atom_str ="";
	int atom_num = 0;
	for (int index = 0; index < chemical.size(); index++)
	{
		//不是数字，也不是字母，例如‘.’,则不处理；
		if (!isalpha(chemical[index]) && !isdigit(chemical[index]))
		{
			return res;
		}
		//是字母
		else if ( isalpha(chemical[index]) )
		{
			//是大写字母则处理，
            //如果atom_str 为空， 那么添加进去，并且获得后面的数字，需要makepair(atom_str,atom_num)
			//如果atom_str 不为空，则当前的string只有一个，需要makepair(atom_str,1)

			if (isupper(chemical[index]))
			{
				if(atom_str.empty())
				{
					atom_str = getStringUpperStart(chemical, index);
					index--;
				}
				else {

				}

			}

		}//是数字
		else
		{
		}

	}

	return res;
}

string getAtomStr(vector<pair<string, int>> atom_pairs, int atom_num) {
	int sum_temp = 0;
	for (int i = 0; i < atom_pairs.size(); i++)
	{
		sum_temp += atom_pairs[i].second;
		
	}

	if (sum_temp < atom_num || atom_num < 1)
	{
		throw("in put atom number error.");
		
	}

	sum_temp = 0;
	for (int i = 0; i < atom_pairs.size(); i++)
	{
		sum_temp += atom_pairs[i].second;

		if (atom_num <= sum_temp) {
			return  atom_pairs[i].first;
		}
	}
}
*/
int getAtomCount(vector<pair<string, int>> atom_pairs)
{
	int atom_count = 0;
	for (int i = 0; i < atom_pairs.size(); i++)
	{
		atom_count += atom_pairs[i].second;
	}
	return atom_count;
}

int getNumber(const string &str, int &index)
{
	int res = 0;
	while (isdigit(str[index]))
	{
		res *= 10;
		res += str[index++] - '0';
	}
	return res;
}


string getStringUpperStart(const string &str, int &index)
{
	string res = "";
	res.push_back(str[index]);
	index++;
	while ( !isupper(str[index]) && !isdigit(str[index]) )
	{ 
		res.push_back(str[index]);
		index++;
	}
	return res;
}


//从string创建图
//图用vector<unordered_set<int>>表示
//比如 graph[1] 表示第一个节点能够连接到那些节点
void buildGraph(string &str, int &index, int prevNode, vector<unordered_set<int>> &graph)
{
	while (index < str.size())
	{
		int node = getNumber(str, index);
		if (prevNode)
		{
			graph[prevNode].insert(node);
			graph[node].insert(prevNode);
		}
		prevNode = node;

		while (str[index] == '-' || str[index] == ' ') index++;
		//if (str[index] == '-') index++;
		while (str[index] == '(')
		{
			index++;
			buildGraph(str, index, prevNode, graph);
		}
		if (str[index] == ')')
		{
			index++;
			return;
		}
		else if (str[index] == ',')
		{
			str[index] = '(';
			return;
		}
	}
}

//主要函数
//str是字符串输入，去掉c的连接关系字符串比如 1-2(3-1)4
//count: 一共有多少个关键元素参与连接
vector<vector<int>> extractCircle(string str, int count)
{
	//创建graph
	count++;
	vector<unordered_set<int>> graph(count);
	int index = 0;
	buildGraph(str, index, 0, graph);

	//从任一点Bfs搜索
	queue<int> que;
	vector<bool> visited(count, false);
	que.push(1);
	visited[1] = true;
	vector<pair<int, int>> keyEdge;

	int circleCount = 0;
	vector<unordered_set<int>> copyGraph = graph;
	while (que.size())
	{
		int size = que.size();
		while (size--)
		{
			int node = que.front();
			que.pop();
			for (int next : copyGraph[node])
			{
				//由于是全向图，防止走回上一个节点
				copyGraph[next].erase(node);
				if (visited[next])
				{
					//如果遇到下一个节点是访问过的节点， 那么当前边必然参与环，存入keyEdge
					keyEdge.push_back(make_pair(node, next));
					circleCount++;
				}
				else
					que.push(next);
				visited[next] = true;
			}
		}
	}

	//准备用keyEdge进行BFS, 将所有关键边断开，graph成为无环树，此时添加任意关键边会产生新环。
	for (auto edge : keyEdge)
	{
		graph[edge.first].erase(edge.second);
		graph[edge.second].erase(edge.first);
	}

	vector < vector<int>> result;
	//循环所有关键边找到其中产生的最小的环，该环必然是一个解，然后恢复对应的关键边，把这个边从keyEdge中删掉，如此往复，直到keyEdge为空
	while (keyEdge.size())
	{
		vector<int> circle;
		int edgeID;//存最优解的edge index
		//依次尝试从每个关键边的A点出发寻找去B点的最短路径(此时该关键边还处于断开状态)，该路径就是在回复该关键边时产生的最小环。
		for (int i = 0; i < keyEdge.size(); i++)
		{
			auto edge = keyEdge[i];
			//if (graph[edge.first].count(edge.second))
			//	continue;
			que = queue<int>();
			vector<vector<int>> paths(count); //记录从点A出发到达每个点的最短路径，如果为空，则表示还没有访问过
			que.push(edge.first);
			paths[edge.first].push_back(edge.first);
			while (que.size())
			{
				int node = que.front();
				que.pop();
				if (node == edge.second)
					break;
				for (auto next : graph[node])
				{
					if (paths[next].empty())//如果没有访问过
					{
						paths[next] = paths[node];//把parent的路径复制过来
						paths[next].push_back(next);//把自己加入路径
						que.push(next);
					}
				}
			}

			if (circle.empty() || paths[edge.second].size() < circle.size())//如果当前的解比最优解要短，更新最优解，记录该关键边的index
			{
				circle = paths[edge.second];
				edgeID = i;
			}
		}
		//记录最优解
		result.push_back(circle);
		//恢复对应关键边的链接
		graph[circle.front()].insert(circle.back());
		graph[circle.back()].insert(circle.front());
		//从KeyEdge中删除
		keyEdge.erase(keyEdge.begin() + edgeID);
	}

	return move(result);
}



string getStrFromUpper(string str, int& i)
{
	string s = "";
	if (!isupper(str[i])) return s;
	s.push_back(str[i]);
	i++;
	while (islower(str[i])) {
		s.push_back(str[i]);
		i++;
	}

	return s;
}

string getStrFromNumber(string str, int& i)
{
	string s = "";
	if (!isdigit(str[i])) return s;

	s.push_back(str[i]);
	i++;
	while (isdigit(str[i])) {
		s.push_back(str[i]);
		i++;
	}

	return s;
}
queue<string> getStrQueue(string str)
{
	queue<string> que_str;
	string s = "";
	string one = "1";
	for (int i = 0; i < str.size(); i++)
	{
		if (isupper(str[i]))
		{
			s = getStrFromUpper(str, i);
			i--;
			que_str.push(s);
			s.clear();
		}
		else if (islower(str[i])) {

		}
		else if (isdigit(str[i]))
		{
			s = getStrFromNumber(str, i);
			i--;
			que_str.push(s);
			s.clear();
		}
		else {
			return que_str;
		}
	}

	return que_str;
}
queue<string> modifyQueue(queue<string> que)
{
	queue<string> que_without_H;
	queue<string>que_add_One;

	while (!que.empty())
	{
		string s = que.front();

		if (s == "H")
		{
			que.pop();

			if (!que.empty())
			{
				string next = que.front();
				if (isalpha(next[0]))
				{
					continue;
				}
				else
				{
					que.pop();
				}
			}
		}

		else {
			que_without_H.push(que.front());
			que.pop();
		}
	}


	while (!que_without_H.empty())
	{
		string s = que_without_H.front();

		if (isalpha(s[0])) {
			que_add_One.push(s);
			que_without_H.pop();
			if (que_without_H.empty()) {
				string one = "1";
				que_add_One.push(one);
				break;
			}

			string next = que_without_H.front();
			if (isalpha(next[0]))
			{
				string one = "1";
				que_add_One.push(one);
			}
			else
			{
				que_add_One.push(next);
				que_without_H.pop();
			}

		}

	}


	return 	que_add_One;
}

vector<string> getAtomName(queue<string> que_str)
{
	vector<string> res;
	string zero_index_string = " Get atom name by Index, index begin with 1.";
	res.push_back(zero_index_string);
	while (!que_str.empty())
	{
		string s = que_str.front();
		que_str.pop();
		int count = stoi(que_str.front());
		que_str.pop();
		while (count--)
		{
			res.push_back(s);
		}

	}


	return res;
}




void main()
{

	string inchi_string;
	cin >> inchi_string;
	InChI inchi_struct = structInChI(inchi_string);
	cout << "version:    " << inchi_struct.version << endl;
	cout << "chemical:  " << inchi_struct.chemical << endl;
	cout << "atomLinkNum: " << inchi_struct.atomLink << endl;

	//vector<pair<string, int>> pairs = getAtomNumPair(inchi_struct.chemical);
	//cout << "number 5 is: " << getAtomStr(pairs, 5) << endl;
	//string test = "1-3-13-7-9-15-11-12-16-10-8-14-4-2-6-18-17(5-1)19(13)21(15)22(16)20(14)18";
	//string test = "1-15-11-17(3-7-21(15)37-39-23-9-5-19-25(55(43,44)45)13-27(57(49,50)51)31(35)29(19)33(23)41)18-4-8-22(16(2)12-18)38-40-24-10-6-20-26(56(46,47)48)14-28(58(52,53)54)32(36)30(20)34(24)42";
	//string test = inchi_struct.atomLink;

	//int count = getAtomCount(pairs);
	//cout << "cout:" << count << endl;

	string test = "1-3-13-7-9-15-11-12-16-10-8-14-4-2-6-18-17(5-1)19(13)21(15)22(16)20(14)18";
	int count = 22;
	auto res = extractCircle(test, count);
	cout << test << endl;
	cout << "RES : " << res.size() << endl;
	for (int i = 0; i < res.size(); i++)
	{
		cout << "circle " << i << " : ";
		for (int e : res[i])
		{
			cout << e << " ";
		}
		cout << endl;
	}


	queue<string> que_str = getStrQueue(inchi_struct.chemical);
	//vector<string > ss = getAtomName(que_str);

	queue<string> que_mod = modifyQueue(que_str);

	vector<string> v_s = getAtomName(que_mod);
	for (auto v : v_s)
	{
		cout << v << endl;
	}
	while (!que_mod.empty())
	{
		cout << que_mod.front() << " ";
		que_mod.pop();
	}
	cout << endl;




	char a;
	cin >> a;
}

vector<string> spliteInChI(string str)
{
	string s = "";
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		s = s + str[i];
	}
	//InChi 应该初始化
	if (s != "InChI")
	{
		throw "InChI is not found.";
	}

	vector<string> str_vec;

	for (int j = 6; j < str.length(); j++)
	{
		s = "";
		while (str[j] != '/' && j < str.length())
		{
			s = s + str[j];
			j++;
		}

		str_vec.push_back(s);

	}

	return str_vec;
}
InChI structInChI(string str)
{
	vector<string> str_vec = spliteInChI(str);
	InChI res;
	res.version = stoi(str_vec[0]);
	res.chemical = str_vec[1];

	for (int i = 0; i < str_vec.size(); i++)
	{


		switch (str_vec[i][0])
		{
		case 'c':
		{
			// remove c from the atom_link_string;   c1-2-3 => 1-2-3
			string atom_link = "";
			for (int j = 1; j < str_vec[i].size(); j++) {
				atom_link = atom_link + str_vec[i][j];
			}
				res.atomLink = atom_link;
			break;
		}
		case 'h':
			res.hydragen = str_vec[i];
			break;
		default:
			break;
		}
	}

	return res;
}
