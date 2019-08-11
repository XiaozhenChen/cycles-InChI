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
		//�������֣�Ҳ������ĸ�����确.��,�򲻴���
		if (!isalpha(chemical[index]) && !isdigit(chemical[index]))
		{
			return res;
		}
		//����ĸ
		else if ( isalpha(chemical[index]) )
		{
			//�Ǵ�д��ĸ����
            //���atom_str Ϊ�գ� ��ô��ӽ�ȥ�����һ�ú�������֣���Ҫmakepair(atom_str,atom_num)
			//���atom_str ��Ϊ�գ���ǰ��stringֻ��һ������Ҫmakepair(atom_str,1)

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

		}//������
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


//��string����ͼ
//ͼ��vector<unordered_set<int>>��ʾ
//���� graph[1] ��ʾ��һ���ڵ��ܹ����ӵ���Щ�ڵ�
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

//��Ҫ����
//str���ַ������룬ȥ��c�����ӹ�ϵ�ַ������� 1-2(3-1)4
//count: һ���ж��ٸ��ؼ�Ԫ�ز�������
vector<vector<int>> extractCircle(string str, int count)
{
	//����graph
	count++;
	vector<unordered_set<int>> graph(count);
	int index = 0;
	buildGraph(str, index, 0, graph);

	//����һ��Bfs����
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
				//������ȫ��ͼ����ֹ�߻���һ���ڵ�
				copyGraph[next].erase(node);
				if (visited[next])
				{
					//���������һ���ڵ��Ƿ��ʹ��Ľڵ㣬 ��ô��ǰ�߱�Ȼ���뻷������keyEdge
					keyEdge.push_back(make_pair(node, next));
					circleCount++;
				}
				else
					que.push(next);
				visited[next] = true;
			}
		}
	}

	//׼����keyEdge����BFS, �����йؼ��߶Ͽ���graph��Ϊ�޻�������ʱ�������ؼ��߻�����»���
	for (auto edge : keyEdge)
	{
		graph[edge.first].erase(edge.second);
		graph[edge.second].erase(edge.first);
	}

	vector < vector<int>> result;
	//ѭ�����йؼ����ҵ����в�������С�Ļ����û���Ȼ��һ���⣬Ȼ��ָ���Ӧ�Ĺؼ��ߣ�������ߴ�keyEdge��ɾ�������������ֱ��keyEdgeΪ��
	while (keyEdge.size())
	{
		vector<int> circle;
		int edgeID;//�����Ž��edge index
		//���γ��Դ�ÿ���ؼ��ߵ�A�����Ѱ��ȥB������·��(��ʱ�ùؼ��߻����ڶϿ�״̬)����·�������ڻظ��ùؼ���ʱ��������С����
		for (int i = 0; i < keyEdge.size(); i++)
		{
			auto edge = keyEdge[i];
			//if (graph[edge.first].count(edge.second))
			//	continue;
			que = queue<int>();
			vector<vector<int>> paths(count); //��¼�ӵ�A��������ÿ��������·�������Ϊ�գ����ʾ��û�з��ʹ�
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
					if (paths[next].empty())//���û�з��ʹ�
					{
						paths[next] = paths[node];//��parent��·�����ƹ���
						paths[next].push_back(next);//���Լ�����·��
						que.push(next);
					}
				}
			}

			if (circle.empty() || paths[edge.second].size() < circle.size())//�����ǰ�Ľ�����Ž�Ҫ�̣��������Ž⣬��¼�ùؼ��ߵ�index
			{
				circle = paths[edge.second];
				edgeID = i;
			}
		}
		//��¼���Ž�
		result.push_back(circle);
		//�ָ���Ӧ�ؼ��ߵ�����
		graph[circle.front()].insert(circle.back());
		graph[circle.back()].insert(circle.front());
		//��KeyEdge��ɾ��
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
	//InChi Ӧ�ó�ʼ��
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
