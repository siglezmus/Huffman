#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <string>
using namespace std;

class Node
{
public:
	int a;
	char c;
	Node *left, *right;

	Node()
	{
		left = right = NULL;
	}

	Node(Node *L, Node *R)
	{
		left = L;
		right = R;
		a = L->a + R->a;
	}
};


struct MyCompare
{
	bool operator()(const Node* l, const Node* r) const { return l->a < r->a; }
};


vector<bool> code;
map<char, vector<bool> > table;

void BuildTable(Node *root)
{
	if (root->left != NULL)
	{
		code.push_back(0);
		BuildTable(root->left);
	}

	if (root->right != NULL)
	{
		code.push_back(1);
		BuildTable(root->right);
	}

	if (root->left == NULL && root->right == NULL) 
		table[root->c] = code;

	if (!code.empty())
		code.pop_back();

}


int main(int argc, char *argv[])
{
	string path,name,out;

	cout << "input full path to .txt file you want to compress" << endl;

	getline(cin, path);

	cout << "input name of .txt(included) you want to compress" << endl;

	getline(cin, name);

	out = path + "output.txt";
	
	path += name;

	cout << endl;

	ifstream f(path, ios::in);
	if (f) 
	{
		map<char, int> itcounter;

		while (!f.eof())
		{
			char c = f.get();
			itcounter[c]++;
		}




		list<Node*> tree;
		//начальные узлы
		for (map<char, int>::iterator itr = itcounter.begin(); itr != itcounter.end(); ++itr)
		{
			Node *p = new Node;
			p->c = itr->first;
			p->a = itr->second;
			tree.push_back(p);
		}


		//////  создаем дерево		

		while (tree.size() != 1)
		{
			tree.sort(MyCompare());

			Node *SonL = tree.front();
			tree.pop_front();
			Node *SonR = tree.front();
			tree.pop_front();

			Node *parent = new Node(SonL, SonR);
			tree.push_back(parent);

		}

		Node *root = tree.front();   //root - указатель на корневой элемент дерева


		BuildTable(root);



		f.clear(); f.seekg(0);

		ofstream g(out, ios::out | ios::binary);

		int count = 0; char buf = 0;
		while (!f.eof())
		{
			char c = f.get();
			vector<bool> x = table[c];
			for (int n = 0; n < x.size(); n++)
			{
				buf = buf | x[n] << (7 - count);
				count++;
				if (count == 8)
				{
					count = 0;   g << buf; buf = 0;
				}
			}
		}

		f.close();
		g.close();
		bool check;
		cout << "If you want to check results input 1" << endl;
		cin >> check;
		cout << endl;
		if (check)
		{
			ifstream result(out, ios::in | ios::binary);

			setlocale(LC_ALL, "Russian");
			cout << "it is decoded output:" << " ";
			Node *p = root;
			count = 0; char byte;
			byte = result.get();
			while (!result.eof())
			{
				bool bit = byte & (1 << (7 - count));
				if (bit) 
					p = p->right;
					else p = p->left;

				if (p->left == NULL && p->right == NULL)
				{
					cout << p->c; p = root;
				}
				count++;
				if (count == 8)
				{
					count = 0; byte = result.get();
				}
			}
			cout << endl;
			result.close();
		}
	}
	else
	{
		cout << "Error reading file" << endl;
	}
	system("pause");

	return 0;
}