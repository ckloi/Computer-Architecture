#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;
class Cache
{
    int tag;
    bool dirty;
    string block[8];
public :

    Cache()
    {
        tag = 0;
        dirty = false;
        for(int x = 0 ; x < 8 ; x++)
        {
            block[x] = "00";
        }

    }


    void setBlock(int index , string value)
    {

        block[index] = value;

    }

    void setTag(int value)
    {

        tag = value;

    }

    void setDirty(bool value)
    {

        dirty = value;

    }

    string getBlock(int index)
    {
        return block[index];
    }

    int getTag()
    {
        return tag;
    }

    bool getDirty()
    {
        return dirty;
    }

    void reset()
    {
        for(int i = 0 ; i < 8 ; i++)
        {
            block[i] = "00";
        }

    }



};

class Memory
{

    string block[8];
public :

    Memory()
    {

        for(int x = 0 ; x < 8 ; x++)
        {
            block[x] = "00";
        }

    }


    void setBlock(int index , string value)
    {

        block[index] = value;

    }

    string getBlock(int index)
    {
        return block[index];
    }

};



void handleMiss(vector<Cache> &caches,vector<Memory> &memory, int lineNum, int memAddress, int tag){
int curMemAddress = caches[lineNum].getTag();
    curMemAddress = curMemAddress << 6;
    curMemAddress = curMemAddress + lineNum;

if(caches[lineNum].getDirty() == true){
   for(int i = 0 ; i < 8; i++){
   memory[curMemAddress].setBlock(i,caches[lineNum].getBlock(i)); 
   }
}
  caches[lineNum].reset();
   
  //bring the correct memory back
  caches[lineNum].setTag(tag);
  for(int i = 0 ; i < 8; i++){
  caches[lineNum].setBlock(i,memory[memAddress].getBlock(i));
  }

  caches[lineNum].setDirty(false);

}

int main(int argc, char* argv[])
{
    int address,tag,lineNum,offset,op,memAddress;
    string data;
    ifstream file(argv[1]);
    ofstream out("dm-out.txt");
    vector<Cache> caches(64);
    vector<Memory> memory(8191);
    bool hit = true;
    bool dirty = false;

    file >> hex;
    while( file >> address)
    {
        tag = (address & 0xFE00) >> 9 ;  // tagNum
        lineNum= (address &0x1F8) >>3 ;  // lineNum
        offset= address & 0x7;   // offset
        file >> op; //get operation : read or write
        file >> data; // get data



        dirty = caches[lineNum].getDirty();
        memAddress = (address >> 3);
        if(caches[lineNum].getTag() != tag){
         hit = false;
         handleMiss(caches,memory,lineNum,memAddress,tag);

        }

        if(op == 0)  //read
        {
            
            out << hex << uppercase << address;
            out << dec << " ";
         for(int i = 7; i >= 0 ;i--){
          out << caches[lineNum].getBlock(i); 
          }
          out << " "  << hit << " " << dirty << endl;


        }
        else   // write
        {

           caches[lineNum].setDirty(true);
           caches[lineNum].setBlock(offset,data);

        }

  
        hit = true;

    }






}
