#include "engineDataBank.h"


#include <iostream>
#include <sstream>
#include <fstream>

DataRow::~DataRow(){ // Destructor...
    rowName.clear();
    rowString.clear();
    columns.clear();
}
DataRow::DataRow(){ //Constructor...
    rowName.clear();
    rowString.clear();
    columns.clear();
}
DataRow::DataRow(string data){
    rowName.clear();
    rowString.clear();
    columns.clear();
    rowSetData( data );
}
void DataRow::rowSetData(string data){
        vector<string> internal; // vector with line parts
        stringstream ss(data); // Splitting line into smaller parts...
        string tok;
        while(getline(ss, tok,' ')) internal.push_back(tok); // splitting job...
        if (internal.size()>0){
            rowName = internal[0];
            if (internal.size()>1)
                for (int a=1; a<internal.size();a++)
                    columns.push_back( internal[a] );
        }
        cout<<"Hi! my name is "<<rowName.c_str()<<" and i got "<<columns.size()<<" args!"<<endl;
}
string DataRow::getName(){ return rowName; }
float DataRow::getColumnFloat(int id) { return float(atof((columns[min(max(0,id), (int(columns.size()))-1)].c_str())));}//atof((  columns[  min( max(0,id),(int(columns.size()))-1  ].c_str())); }
string DataRow::getColumnString(int id){ return columns[min(max(0,id), (int(columns.size()))-1)]; }
int DataRow::getColumnInt(int id) { return int(atof((columns[min(max(0,id), (int(columns.size()))-1)].c_str()))); }


//------------------------------------------------------------
DataFile::DataFile(){ //Constructor...
    pathName.clear();
    fileName.clear();
    rows.clear();
    rowsMap.clear();
}
DataFile::DataFile(string filename){
    //string fpatch = fname+".txt";
    ifstream file( filename.c_str() );
    string line;
    while(getline(file, line))
        addRow(line);
    buildMap();
}
DataFile::~DataFile(){ //Destructor...

}
void DataFile::addRow( string line ){
        rows.push_back( new DataRow(line) );
};
int DataFile::getRowCount(){ return rows.size(); }
DataRow* DataFile::getRow( int id ){ return rows[min(max(0,id),((int)rows.size())-1)]; }
DataRow* DataFile::getRow( string name ){
    DataRow* r = NULL;
     if ( rowsMap.find(name) != rowsMap.end() ){
        r = rowsMap[name];
     }
    return r;
};
void DataFile::buildMap(){
    rowsMap.clear();
    for (int i = 0 ; i <rows.size() ; i++ ){
        DataRow* dr = (DataRow*)rows[i];
        rowsMap[dr->getName()] = dr;
    }

};
