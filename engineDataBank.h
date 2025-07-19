#ifndef DATA_BANK_H
#define DATA_BANK_H
#include <map>
#include "engine.h"


class DataRow
{
    private:
        string rowName;
        string rowString;
        vector<string> columns;
    public:

        DataRow();
        DataRow(string data);

        ~DataRow();

        void rowSetData( string data ); // Same as constructor;
        string rowGetData();

        int getColumnsCount();
        string getName();
        float getColumnFloat(int id);
        int getColumnInt(int id);
        string getColumnString(int id);
};

class DataFile // contains all data , rows and columns
{
    private:
        string pathName;
        string fileName;
        //DataRow* nullRow;
        vector< DataRow* > rows;
        map<string,DataRow*> rowsMap; // mapped rows by names...
    public:

        DataFile();
        DataFile(string filename);
        ~DataFile();

        int getRowCount();
        void addRow( string data );
        DataRow* getRow( int id );
        DataRow* getRow( string name );

        //read( string filename );
        void buildMap();//build map...

};
/*

class data_slot
{
    public:
        data_slot( string ); //max_quads
        ~data_slot();
        string name;
        vector<string> args;

        //string get_count();

        //string get_string( int );
        //int get_int( int );
        //float get_float( int );
        //Vec2 get_vector( int );
};

class data_bank
{
    public:

        data_bank(); //max_quads
        ~data_bank();

        string file_patch;

        vector< data_slot* > data_vector;
        map<string,data_slot*> data_map; // mapped textures by string

        int get_size();
        data_slot* get_data( int id);
        void add_data( string line );
        void load_from_file( string );
        void build_map();

        //vector get_vector();
        data_slot* find_data( string );


};
*/
#endif // DATA_BANK_H
