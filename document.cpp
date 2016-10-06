#include "stem_statistics.h"
#include "document.h"
#include <stack>
#include <algorithm>
#include <cstdio>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include "porter2_stemmer.h"
#include <map>
#include <cmath>

#include <chrono> // this is for check execution time
#include <ctime> // this is for check execution time

list<string> Document::stopwords = {};
string Document::outputDirectory = "";
int Document::idMaker = 0;
int WordInfo::lastId = 0;
map<string, int> Document::collectionFrequencies;
map<string, int> Document::documentFrequencies;

Document::Document(string _docno, string headline, string text) {
	id = ++idMaker;
	docno = _docno;
	removePunctuation(headline);
	removePunctuation(text);
	headlineWords = tokenize(headline);
	textWords = tokenize(text);
}

list<string> Document::tokenize(string str) {
	list<string> result;
	char * c_str = strdup(str.c_str());
	char * tokenizer = " -.,\"'\n\t:";
	//char* tokenizer = " -\n\t,.:_()'`\"/";

	for(char * ptr = strtok(c_str, tokenizer); ptr != NULL; ptr = strtok(NULL, tokenizer)) {
		string temp = string(ptr);
		::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
		if(isStopword(temp))
			continue;
		result.push_back(temp);
	}

	free(c_str);
	return result;
}

void Document::addWord(string temp) {
	collectionFrequencies[temp]++;

	// 보고서에 써야징 - 여기서 하면 매번 모든 key를 검색하니 비효율. 나중에 한번에 df 추가하는게 나음.
	/*if(document.termFrequencies[temp] == 0) {
		df[wordId]++;
	}*/

	if(maxFrequency < ++termFrequencies[temp]) {
		maxFrequency = termFrequencies[temp];
	}
}

void Document::addDf() {
	map<string, int>::iterator iter = termFrequencies.begin();
	while( iter != termFrequencies.end()) {
		documentFrequencies[iter->first]++;
		iter++;
	}
}

void removePunctuation( string &str ) {
	char* charsToRemove = "?()`;*$"; // <>
	for (unsigned int  i = 0; i < strlen(charsToRemove); ++i) {
		str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}

void Document::transform() {
	removeNumberWords(headlineWords);
	removeNumberWords(textWords);
	stem(headlineStems, headlineWords);
	stem(textStems, textWords);

	//headlineWords.unique();
	//textStems.unique();
}

void removeNumberWords( list<string> &words ) {
	regex reg(".*[0-9]+.*");
	list<string>::iterator iter = words.begin();
	while( iter != words.end()) {
		if(regex_match(*iter, reg)) {
			iter = words.erase(iter);
		} else
			iter++;
	}
}

bool isStopword(string word) {
	list<string>::iterator iter = Document::stopwords.begin();
	while( iter != Document::stopwords.end()) {
		if(word == *iter)
			return true;
		iter++;
	}
	return false;
}

void Document::stem(list<string> &stemList, list<string> words) {
	list<string>::iterator iter = words.begin();
	while( iter != words.end()) {
		string word = *iter;
		Porter2Stemmer::trim(word);
		Porter2Stemmer::stem(word);
		// TODO 옮기자!!!!!
		if(!word.empty()) {
			stemList.push_back(word);
		}
		addWord(word);
		//addStemStatistics(*iter, word);
		iter++;
	}
}

// tokenize 과정에서 처리해주는 것으로 보임. 나중에 삭제
// Deprecated
void trim(string &str) {
	string whitespaces (" \t\f\v\n\r");
	// trim right
	size_t found = str.find_last_not_of(whitespaces);
	if (found!=string::npos) {
		str.erase(found+1);
	}else {
		str.clear();
	}

	// trim left
	str.erase(0, str.find_first_not_of(whitespaces));
}


string Document::to_string() {
	string result = "";
	result += "[DOCNO] : " + docno + "\n";
	result += "[HEADLINE] : " + concatStringList(headlineStems) + "\n";
	result += "[TEXT] : " + concatStringList(textStems) + "\n";

	return result;
}

void Document::makeDocInfoFile() {
	ofstream outputFile (outputDirectory + "/doc.dat", ios_base::app);
	outputFile << id << "\t" << docno << "\t" << termFrequencies.size() << "\t" << denominator << endl;
	outputFile.close();
	// 통계내기위한 코드
	//writeHighRankedTfIdfWords(documentList);
	//writeStemStatistics();
}

void Document::calculateDenominator(float dValue) { // tf idf formula's denominator
	map<string, int>::iterator iter = termFrequencies.begin();
	while( iter != termFrequencies.end()) {
		denominator += pow((log(iter->second) + 1.0f) * dValue, 2.0f);
		iter++;
	}
	denominator = sqrt(denominator);
}
void Document::writeTFFile() {
	ofstream outputFile (outputDirectory + "tf.dat", ios_base::app);
	map<string, int>::iterator iter = termFrequencies.begin();
	while( iter != termFrequencies.end()) {
		outputFile << id << "\t" << docno << "\t" << iter->first << "\t" << iter->second << endl;
		iter++;
	}
	outputFile.close();
}

/*

float Document::termFrequency(string term) {
	//augmented frequency, to prevent a bias towards longer documents
	return 0.5f + (0.5f * (float)termFrequencies[term] / (float)maxFrequency);
}

bool Document::contain(string term) {
	return termFrequencies.find(term) != frequencies.end();
}

float Document::idf(string term, list<Document> documents) {
	int termAppearedDocumentNumber = 0;
	list<Document>::iterator iter = documents.begin();
	while( iter != documents.end()) {
		if(iter->contain(term))
			termAppearedDocumentNumber++;
		iter++;
	}

	return log10(documents.size() / (termAppearedDocumentNumber + 1));
}

float Document::tfidf(string term, list<Document> documents) {
	return termFrequency(term) * idf(term, documents);
}

void writeHighRankedTfIdfWords(list<Document> documentList) {
	ofstream outputFile ("tfidf");
	list<Document>::iterator iter = documentList.begin();
	while( iter != documentList.end()) {
		map<string, int>::iterator wordIter = iter->termFrequencies.begin();
		while( wordIter != iter->termFrequencies.end()) {
			float tfidf = iter->tfidf(wordIter->first, documentList);
			if(tfidf < 0.2)
				outputFile << wordIter->first << " : " <<  tfidf << endl;
			wordIter++;
		}
		iter++;
	}
	outputFile.close();

}*/
