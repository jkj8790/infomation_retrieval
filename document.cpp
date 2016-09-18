#include <stack>
#include <cstdio>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include "porter2_stemmer.cpp"
#include <map>

#include <chrono> // this is for check execution time
#include <ctime> // this is for check execution time
#include "stem_statistics.cpp"

void removePunctuation( string &str );
void removeNumberWords( list<string> &words );
void removeStopword(list<string> &words);
void stemWordList(list<string> &words);

class Document {
  public:
    static list<string> stopwords;
    string docno;
    list<string> headlineWords;
    list<string> textWords;
    map<string, int> frequencies;
    list<string> headlineStems;
    list<string> textStems;
    int maxFrequency = 0;

    Document(string _docno, string headline, string text) {
      docno = _docno;
      removePunctuation(headline);
      removePunctuation(text);
      headlineWords = tokenize(headline);
      textWords = tokenize(text);
    }

    list<string> tokenize(string str);
    float tf(string term);
    float idf(string term, list<Document> documents);
    float termFrequency(string term);
    bool contain(string term);
    void transform();

    //TODO ignoreCase 하게 짜야함
    string to_string();
};

list<string> Document::stopwords = {};

list<string> Document::tokenize(string str) {
  cout << docno << endl;
  list<string> result;
  istringstream iss(str);
  do {
    string temp;
    iss >> temp;
    // 메소드로 분리하는게 좋을듯
    if(maxFrequency < ++frequencies[temp]) {
      maxFrequency = frequencies[temp];
      cout << temp << " : " << frequencies[temp] << endl;
    }
    result.push_back(temp);
  } while(iss);
  return result;
  
  /*
  if(iter != stemStatisticsList.end()) {
    iter->addCount(word);
  } else {
    StemStatistics stemStatistics (stem);
    stemStatistics.addCount(word);
    stemStatisticsList.push_back(stemStatistics);
  }*/
}

void removePunctuation( string &str ) {
  char* charsToRemove = "?()'`\",.;_";
  for (unsigned int  i = 0; i < strlen(charsToRemove); ++i) {
    str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end());
  }
}

void Document::transform() {
  removeNumberWords(headlineWords);
  removeNumberWords(textWords);
  removeStopword(headlineWords);
  removeStopword(textWords);
  stemWordList(headlineWords);
  stemWordList(textWords);
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

void removeStopword(list<string> &words) {
  list<string>::iterator iter = Document::stopwords.begin();
  while( iter != Document::stopwords.end()) {
    words.remove(*iter);
    iter++;
  }
}

void stemWordList(list<string> &words) {
  list<string>::iterator iter = words.begin();
  while( iter != words.end()) {
    string word = *iter;
    Porter2Stemmer::trim(*iter);
    Porter2Stemmer::stem(*iter);
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
  result += "[HEADLINE] : " + concatStringList(headlineWords) + "\n";
  result += "[TEXT] : " + concatStringList(textWords) + "\n";
  return result;
}

float Document::termFrequency(string term) {
  //frequencies[temp] / 
}
