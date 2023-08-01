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
		if (a->freq == b->freq) {
			return a > b;
		}
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
		compressed += huffmanCodes[c];
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



void stringToBinaryFile(const string& inputString, const string& filename) {
	ofstream binaryFile(filename, ios::binary);

	if (!binaryFile) {
		cerr << "Error opening the binary file." << endl;
		return;
	}

	unsigned char currentByte = 0;
	int bitCount = 0;

	for (char c : inputString) {
		if (c == '1') {
			currentByte |= (1 << (7 - bitCount));
		}

		bitCount++;

		if (bitCount == 8) {
			binaryFile.write(reinterpret_cast<const char*>(&currentByte), 1);
			bitCount = 0;
			currentByte = 0;
		}
	}

	if (bitCount > 0) {
		binaryFile.write(reinterpret_cast<const char*>(&currentByte), 1);
	}

	binaryFile.close();
	cout << "Binary data written to " << filename << " successfully." << endl;
}

string binaryFileToString(const string& filename) {
	ifstream binaryFile(filename, ios::binary);

	if (!binaryFile) {
		cerr << "Error opening the binary file." << endl;
		return "";
	}

	string binaryString;
	unsigned char currentByte;

	while (binaryFile.read(reinterpret_cast<char*>(&currentByte), 1)) {
		bitset<8> bits(currentByte);
		binaryString += bits.to_string();
	}

	binaryFile.close();
	return binaryString;
}

void stringToFile(const string& dataString, const string& filename) {
	ofstream outputFile(filename);

	if (!outputFile) {
		cerr << "Error opening the output file." << endl;
		return;
	}

	outputFile << dataString;
	outputFile.close();
	cout << "Data written to " << filename << " successfully." << endl;
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

	/*for (const auto& i : huffmanCodes) {
		cout << i.first << " " << i.second << endl;
	}*/

	string compressed = compressString(input, huffmanCodes);



	stringToBinaryFile(compressed, "Compressed.bin");
	string binaryString = binaryFileToString("Compressed.bin");

	string decompressed = decompressString(binaryString, root);

	stringToFile(decompressed, "decopressed.txt");

	delete root;
	return 0;
}
