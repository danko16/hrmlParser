#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <queue>
using namespace std;

class Elements;
class HierarchyTag;

struct Node {
	Elements *el;
	vector<Node*> child;
};

class Elements {
public:
	string tag;
	vector<vector<string>> attributes;

	void addTag(string tag) {
		this->tag = tag;
	}

	void addAttributes(vector<vector<string>> attributes) {
		this->attributes = attributes;
	}

	string getValue(string key) {
		string value = "Not Found!";
		for (unsigned int i = 0; i < attributes.size(); i++) {
			if (key == attributes[i][0]) {
				value = attributes[i][1];
			}
		}

		return value;
	}
};

class HierarchyTag {
private:
	Node *root;
public:
	HierarchyTag() {
		root = NULL;
	}

	void insertRoot(Elements **el) {
		Node *temp = new Node;
		temp->el = *el;
		root = temp;
	};

	void insertChild(Elements **el, int depth) {
		Node *temp = new Node;
		temp = root;
		while (depth > 1)
		{
			temp = temp->child[0];
			depth--;
		}

		Node *nextChild = new Node;
		nextChild->el = *el;
		temp->child.insert(temp->child.begin(), nextChild);
	};

	void insertNode(Node* node) {
		root->child.push_back(node);
	}

	void displayElement() {
		displayElement(root);
	};

	void matchQuery(queue<string> query, string key) {
		matchQuery(root, query, key);
	};

	void displayElement(Node* root, unsigned int index = 0) {
		while (index < root->child.size())
		{
			cout << root->child[index]->el->tag << endl;
			if (root->child[index]->child.size() != 0) {
				displayElement(root->child[index], 0);
			}
			index++;
		}
	};

	void matchQuery(Node* root, queue<string> &gq, string key, unsigned int index = 0) {
		string value;

		if (root->child.empty()) {
			gq.pop();
			value = "Not Found!";
			cout << value << endl;
			return;
		};

		while (index < root->child.size())
		{

			if (!gq.empty() && root->child[index]->el->tag == gq.front()) {
				gq.pop();
				if (!gq.empty()) {
					matchQuery(root->child[index], gq, key, 0);
				}
				else {
					value = root->child[index]->el->getValue(key);
					cout << value << endl;
					return;
				}
			}

			if (gq.empty()) return;

			index++;

			if (index >= root->child.size()) {
				queue<string> empty;
				swap(gq, empty);
				value = "Not Found!";
				cout << value << endl;
				return;
			}
		}
	}

	void getValue(vector<string> query) {
		queue<string> gq;
		string key;
		for (unsigned int i = 0; i < query.size(); i++) {
			if (query[i] == ".") {
				gq.push(query[i - 1]);
			}

			if (query[i] == "~") {
				gq.push(query[i - 1]);
				key = query[i + 1];
			}
		}

		matchQuery(gq, key);
	}

	Node* getRoot() {
		return root;
	}

	vector<Node*> getChildern() {
		return root->child;
	}
};

void inputLength(ifstream &file, int &totalTag, int &totalQuery) {
	string line;
	getline(file, line);

	istringstream ss(line);
	int number, trackStream = 1;
	while (ss >> number) {
		if (trackStream == 1) {
			totalTag = number;
		}

		if (trackStream == 2) {
			totalQuery = number;
		}

		trackStream++;
	}

	trackStream = 1;
}


void inputTags(ifstream &file, int totalTag, vector<string> &tags) {
	for (int i = 0; i < totalTag; i++) {
		string tag;
		getline(file, tag);
		tags.push_back(tag);
	}
}

void inputQuery(ifstream &file, int totalQuery, vector<string> &queries) {
	for (int i = 0; i < totalQuery; i++) {
		string query;
		getline(file, query);
		queries.push_back(query);
	}

}


void parseHrml(vector<string> tags, vector<string> &parsedTags) {
	for (unsigned int i = 0; i < tags.size(); ++i) {
		string word = "";
		for (auto tag : tags[i]) {
			if (tag == ' ' || tag == '<' || tag == '>' || tag == '/' || tag == '"' || tag == '\t') {
				if (tag == '<') {
					parsedTags.push_back("<");
				}

				if (tag == '/') {
					parsedTags.push_back("/");
				}

				if (word != "") {
					parsedTags.push_back(word);
				}

				if (tag == '>') {
					parsedTags.push_back(">");
				}

				word = "";
			}
			else {
				word += tag;
			}
		}
	}

}

void parseQuery(vector<string> queries, vector<vector<string>> &selectedQuery) {
	for (unsigned int i = 0; i < queries.size(); i++) {
		string word = "";
		vector<string> selected;
		for (auto query : queries[i]) {
			if (query == '.' || query == '~' || query == ' ' || query == '\t') {
				if (word != "") selected.push_back(word);
				if (query == '.') selected.push_back(".");
				if (query == '~') selected.push_back("~");

				word = "";
			}
			else {
				word += query;
			}
		}

		selected.push_back(word);
		selectedQuery.push_back(selected);
	}
}

void collectElements(
	vector<string> parsedTags,
	vector<Elements*> &elements,
	vector<string> &closingTags
) {
	vector<vector<string>> attributes;
	string tag = "";
	for (unsigned int i = 0; i < parsedTags.size(); i++) {

		if (parsedTags[i] == "<" && parsedTags[i + 1] != "/") {
			tag = parsedTags[i + 1];
		}

		if (parsedTags[i] == "=") {
			string key = parsedTags[i - 1];
			string value = parsedTags[i + 1];

			vector<string> obj;
			obj.push_back(key);
			obj.push_back(value);

			attributes.push_back(obj);
		}

		if (parsedTags[i] == ">" && parsedTags[i - 2] != "/") {
			Elements *el = new Elements;
			el->addTag(tag);
			el->addAttributes(attributes);

			elements.push_back(el);
			attributes.clear();
		}

		if (parsedTags[i] == "/") {
			closingTags.push_back(parsedTags[i + 1]);
		}
	}
}

void arrangeTags(
	vector<HierarchyTag*> &hrVector,
	stack<HierarchyTag*> &hrStack,
	vector<Elements*> elements,
	vector<string> closedTags,
	int startTag = 0,
	int closingTag = 0,
	int childern = 0,
	int depth = 0
) {
	if (unsigned(closingTag) >= closedTags.size() || unsigned(startTag) >= elements.size()) {
		return;
	}

	if (elements[startTag]->tag == closedTags[closingTag]) {
		if (closingTag - startTag == childern) {
			HierarchyTag *temp = new HierarchyTag();
			temp->insertRoot(&elements[startTag]);
			hrStack.push(temp);
			arrangeTags(hrVector, hrStack, elements, closedTags, startTag + (childern + 1), closingTag + 1, 0, 0);
		} else if (startTag < closingTag && depth == 0) {
			arrangeTags(hrVector, hrStack, elements, closedTags, startTag - 1, closingTag, childern, depth);
		} else if (startTag == closingTag && childern != 0) {
			arrangeTags(hrVector, hrStack, elements, closedTags, startTag - childern, closingTag + 1, childern + 1, depth - 1);
			hrStack.top()->insertChild(&elements[startTag], depth);
		} else {
			arrangeTags(hrVector, hrStack, elements, closedTags, startTag - 1 , closingTag + 1, childern + 1, depth - 1);
			hrStack.top()->insertChild(&elements[startTag], depth);
		}

		if (childern == 0) {
			hrVector.push_back(hrStack.top());
			hrStack.pop();
		}
	}
	else {
		if (closingTag - startTag == childern) {
			startTag = startTag + childern + 1;
			depth++;
		}
		else if (startTag < closingTag) {
			startTag--;
		}
		else {
			startTag++;
			depth++;
		}
		arrangeTags(hrVector, hrStack, elements, closedTags, startTag, closingTag, childern, depth);
	}
}

int main()
{
	ifstream file("input.txt");

	int totalTag, totalQuery;

	HierarchyTag* root = new HierarchyTag;
	Elements* temp = new Elements;
	temp->addTag("<>");
	root->insertRoot(&temp);

	vector<string> tags, queries, parsedTags, closedTags;
	vector<Elements*> elements;
	vector<vector<string>> selectedQuery;
	vector<HierarchyTag*> hrVector;
	stack <HierarchyTag*> hrStack;

	inputLength(file, totalTag, totalQuery);
	inputTags(file, totalTag, tags);
	inputQuery(file, totalQuery, queries);

	file.close();

	parseHrml(tags, parsedTags);
	parseQuery(queries, selectedQuery);

	collectElements(parsedTags, elements, closedTags);
	arrangeTags(hrVector, hrStack, elements, closedTags);

	for (int i = hrVector.size() - 1; i >= 0; --i) {
		root->insertNode(hrVector[i]->getRoot());
	}

	for (int i = 0; i < selectedQuery.size(); i++) {
		root->getValue(selectedQuery[i]);
	}

	return 0;
}
