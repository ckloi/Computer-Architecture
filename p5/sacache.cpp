#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <list>
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


class Set{

list <int> lru;
Cache caches[4];






public:
Set(){

for(int i = 0; i < 4; i++){
   lru.push_back(i);
}

}

int searchTag(int tag){
  for(int i = 0; i < 4 ; i++){
   if( caches[i].getTag() == tag){
        return i;
    }
  }

  return -1;
}

bool getDirty(int pos){

   return caches[pos].getDirty();

}

void setDirty(int pos, bool value){

   caches[pos].setDirty(value);

}

void setTag(int pos, int value){

   caches[pos].setTag(value);

}

int getTag(int pos){

   return caches[pos].getTag();

}



void setBlock(int lineNum ,int index , string value){

caches[lineNum].setBlock(index,value);

}


string getBlock(int lineNum , int index){

return caches[lineNum].getBlock(index);

}

int lruOperation(){
 int front = lru.front();
 lru.push_back(lru.front());
 lru.pop_front();
 return front;
}
void lruOperation(int value){
/*
cout << value << endl;
for(list<int>::iterator it = lru.begin(); it != lru.end(); ++it){
    cout << *it << " ";
}
*/
//cout << endl;
lru.remove(value);
lru.push_back(value);

}



};





int handleMiss(vector<Set> &sets,vector<Memory> &memory, int setNum, int memAddress, int tag){

   int lineNum = sets[setNum].lruOperation();
   int curMemAddress =  (sets[setNum].getTag(lineNum))<< 4;
       curMemAddress = curMemAddress + setNum;
   if(sets[setNum].getDirty(lineNum) == true){
      for(int i = 0; i < 8 ; i++){
      memory[curMemAddress].setBlock(i,sets[setNum].getBlock(lineNum,i));
      }        
    }
  
    
    sets[setNum].setTag(lineNum,tag);
    for(int i = 0; i < 8 ; i++){
      sets[setNum].setBlock(lineNum,i,memory[memAddress].getBlock(i));
      } 


     return lineNum;
}

int main(int argc, char* argv[])
{
    int address,tag,setNum,offset,op,memAddress;
    string data;
    ifstream file(argv[1]);
    ofstream out("sa-out.txt");
    vector<Set> sets(16);
    vector<Memory> memory(8191);
    bool hit = true;
    bool dirty = false;

    file >> hex;

    
    while( file >> address)
    {
        int pos;
        tag = address >> 7 ;  // tagNum
        setNum= (address &0x78) >> 3 ;  // setNum
        offset= address & 0x7;   // offset
        file >> op; //get operation : read or write
        file >> data; // get data

         
         memAddress = (address >> 3);
         pos = sets[setNum].searchTag(tag);
       
         //cout << pos;
        if(pos == -1){
          hit = false;
          pos = handleMiss(sets,memory,setNum,memAddress,tag);
          dirty = sets[setNum].getDirty(pos);
          sets[setNum].setDirty(pos,false);
        }else{

         sets[setNum].lruOperation(pos);
         dirty = sets[setNum].getDirty(pos);
        }
       
        
        

        
        if(op == 0)  //read
        {
            
            out << hex << uppercase;
            if(address < 16){
             out<< 0 << 0 << 0;
            }else if(address < 256){
             out<< 0 << 0;
            }else if(address < 4096){
             out<< 0;
            }
            out << address;
            out << dec << " ";
         for(int i = 7; i >= 0 ;i--){
          out << sets[setNum].getBlock(pos,i); 
          }
          out << " "  << hit << " " << dirty << endl;
         

        }
        else   // write
        {
             
           sets[setNum].setDirty(pos,true);
           sets[setNum].setBlock(pos,offset,data);
          

        }

  
        hit = true;

    }






}
