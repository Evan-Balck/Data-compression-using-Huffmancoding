#include<iostream>
#include<queue>
#include<unordered_map>
#include<fstream>
#include <bitset>

using namespace std;

struct HuffmanTree {
	char data;
	int freq;
	HuffmanTree* left;
	HuffmanTree* right;

	HuffmanTree(char data, int freq) {
		this->data = data;
		this->freq = freq;
		left = nullptr;
		right = nullptr;
	}
};

struct Compare {
	bool operator()(HuffmanTree* a, HuffmanTree* b) {
		return a->freq > b->freq;
	}
};

HuffmanTree* BuildTree(unordered_map<char, int>& freq) {
	priority_queue<HuffmanTree*, vector<HuffmanTree*>, Compare> pq;
	for (auto pair : freq) {
		pq.push(new HuffmanTree(pair.first, pair.second));
	}

	while (pq.size() > 1) {
		HuffmanTree* left = pq.top();
		pq.pop();
		HuffmanTree* right = pq.top();
		pq.pop();
		int combinedFreq = left->freq + right->freq;
		HuffmanTree* newNode = new HuffmanTree('\0', combinedFreq);
		newNode->left = left;
		newNode->right = right;

		pq.push(newNode);
	}

	return pq.top();
}

void buildCodes(HuffmanTree* root, string code, unordered_map<char, string>& huffmanCodes) {
	if (root == nullptr) {
		return;
	}

	if (root->data != '\0') {
		huffmanCodes[root->data] = code;
	}

	buildCodes(root->left, code + "0", huffmanCodes);
	buildCodes(root->right, code + "1", huffmanCodes);
}

string compressString(string& input, unordered_map<char, string>& huffmanCodes) {
	string compressed;
	for (char c : input) {
		compressed += huffmanCodes.at(c);
	}
	return compressed;
}

string decompressString(const string& compressed, HuffmanTree* root) {
	string decompressed;
	HuffmanTree* currentNode = root;

	for (char bit : compressed) {
		if (bit == '0') {
			currentNode = currentNode->left;
		}
		else {
			currentNode = currentNode->right;
		}

		if (currentNode->left == nullptr && currentNode->right == nullptr) {
			decompressed += currentNode->data;
			currentNode = root;
		}
	}

	return decompressed;
}



int main() {
	string file = "Data.txt";

	string input;
	ifstream inputFile(file);
	if (inputFile.is_open()) {
		char c;
		while (inputFile.get(c)) {
			input += c;
		}
		inputFile.close();
	}
	else {
		cerr << "Error: Unable to open the file " << file << endl;
		return 1;
	}

	unordered_map<char, int> frequency;
	for (char c : input) {
		frequency[c]++;
	}

	HuffmanTree* root = BuildTree(frequency);

	unordered_map<char, string> huffmanCodes;
	buildCodes(root, "", huffmanCodes);

	for (const auto& i : huffmanCodes) {
		cout << i.first << " " << i.second << endl;
	}

	string compressed = compressString(input, huffmanCodes);

	string decompressed = decompressString(compressed, root);
	cout << "Decompressed Output:" << endl;
	cout << decompressed << endl;


	delete root;
	return 0;
}
