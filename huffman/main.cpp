#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstddef>
#include <bitset>
#include <queue>
#include <unordered_map>
#include<string>

using namespace std;
///node structure used for building huffman tree
typedef struct node{
    struct node* left;
    struct node* right;
    int  freq;
    char c;
    node( char data, unsigned freq)
    {
        left = right = NULL;
        this->c = data;
        this->freq = freq;
    }

}huffman_tree_node;
///////////////////// //////////////////

//function used to build the huffman codes hashmap recursively from the huffman tree
void printCodes(huffman_tree_node * root, string code,unordered_map<int,string> &numbers)
{
    if (!root)
        return;

    if (root->c != -1){
        //adding the leaf node to the hashmap of huffman codes
        numbers[root->c]=code;
        //printing the code table
        cout<<(int) root->c<<"\t\t"<<bitset<8>(root->c)<<"\t"<<numbers[root->c]<<endl;
        
    }

    printCodes(root->left,code+"1",numbers);
    printCodes(root->right, code+"0",numbers);

}

//function used to build the huffman tree from the frequencies prority queue
void HuffmanCodes(priority_queue<pair<int, huffman_tree_node* >> &minHeap,unordered_map<int,string> &numbers)
{
    huffman_tree_node *left, *right, *top;
    
    while (minHeap.size() != 1) {

        left = minHeap.top().second;
        minHeap.pop();
        right = minHeap.top().second;
        minHeap.pop();

        top = new huffman_tree_node(-1, left->freq + right->freq);

        top->left = left;
        top->right = right;

        minHeap.push(make_pair(top->freq*(-1),top));
    }
    cout<<"codes used to compress the file:"<<endl;
    cout<<"Byte"<<"\t\t"<<"Code"<<"\t"<<"New Code"<<endl;
    //call for printcodes function to generate the huffman codes hash map from the tree
    printCodes(minHeap.top().second,"",numbers);

}

//function used to read character frequencies whether from folder as a whole
//or a file
void read_frequencies(priority_queue<pair<int, huffman_tree_node * >> &input_priority_queue,string &main_message ,vector<int> &file_message_lengths,char* input_location,char file_folder,vector<string> &file_names, long long &input_file_sizes){
    //map for input frequencies
    unordered_map<char,int> input_char_frequencies;
    //variable to get the size of input
    input_file_sizes = 0;
    //if the input is file
    if(file_folder=='1'){
        int count = 0;
        ifstream input_file;
        main_message="";
        input_file.open(input_location);
        string temp(input_location);
        temp = temp.substr(temp.find_last_of("\\/"),*(temp.end()-1));
        //adding the name of the file to the file named vector
        file_names.push_back(temp);
        char input_character;
        //looping on the file and adding the data to the main message
        while (input_file.get(input_character)) {
        input_char_frequencies[input_character]++;
            main_message+=input_character;
            
            count++;
        }
        //saving the size of the input file
        input_file_sizes = std::__fs::filesystem::file_size(input_location);
        input_file.close();
        //storing number of chracters in the file
        file_message_lengths.push_back(count);
    }else{//if input is a folder
         main_message="";
        //looping over all the files in folder
        for (const auto& file : std::__fs::filesystem::directory_iterator{input_location})  //loop through the current folder
        {  int count = 0;
            //opening the file
            std::ifstream input_file{ file.path() };    //open the file
            char input_character;
           // reading all the characters and adding them to the main message and counting its size and storing it
            while (input_file.get(input_character)) {
            input_char_frequencies[input_character]++;
            main_message+=input_character;
            count++;
            }
            
            input_file.close();
            //adding the file size to the total folder size
            input_file_sizes += std::__fs::filesystem::file_size(file.path().c_str());
            file_message_lengths.push_back(count);
            string temp(file.path());
            temp = temp.substr(temp.find_last_of("\\/"),*(temp.end()-1));
            cout<<temp<<endl;
            file_names.push_back(temp);
             
            
        }}
    
    // adding all the characters with therre frequencies to the priority queue
    for (auto &freq_pair:input_char_frequencies) {
        if (freq_pair.second != 0) {
            input_priority_queue.push(make_pair(freq_pair.second * (-1), new huffman_tree_node(freq_pair.first, freq_pair.second)));
            
        }
    }
    
}
//functionused to output the compressed file
void output_compressed_file(unordered_map<int, string> &huffman_codes,char* file_location,char file_folder,string &main_message,  vector<int> &file_message_lengths,vector<string> &file_names,long long &output_file_size){
    
    int count;
    string path;

    vector<bitset<1>> output_binary;//vector of bitset to hold the binary message
    int padding=0;
    int k=0;
   //converting the main message to its binary huffman code
    for(auto &it:main_message){
        k++;
        string x = huffman_codes[it];
        for(int i=0;i<x.size();i++){
        bitset<1> b(x[i]);
            output_binary.push_back(bitset<1>(b));
        }
        
        }
    //padding the binary meassage by zeros to be able to divide it to unsigned chars
      while(output_binary.size()%8!=0){
          bitset<1>temp(0);
          output_binary.push_back(temp);
          padding++;
      }
    
       bitset<8>output_bytes;
    //vector of unsigned characters to wrte to compressed file
    vector<unsigned  char> output_characters;
         count=7;
         cout<<endl;
    //converting the binary string to vector of unsigned characters to write them to the //compressed file
         for(auto it=output_binary.begin();it<=output_binary.end();it++){
             if(count==-1){
                 char c= char(output_bytes.flip().to_ulong());
                 output_characters.push_back(c);
               
                 count=7;
             }
             if(*it==0)
                 output_bytes.set(count);
             else if(*it==1){
                 output_bytes.set(count,0);
             }
             count--;
         }
  
    //////////////////////////////////
    
    
    ofstream output_file;
    //determining the output directory whether the compressed is file or a foldder
    if(file_folder=='2'){
        path ="/compressed_folder.txt";
        string temp (file_location);
        temp = temp.substr(0,temp.find_last_of("\\/"));
        path = temp+path;
        output_file.open(path.c_str());}
    else if(file_folder=='1'){
     path = file_location;
        output_file.open(path);
    }
 //writing the header of the compressed file
    output_file<<file_message_lengths.size();//number of files
    output_file<<",";

    for(int i=0;i<file_message_lengths.size();i++)//size of each file
    {output_file<<file_message_lengths[i];
        output_file<<",";}
    
    for(int i=0;i<file_names.size();i++)//name of each file
    {output_file<<file_names[i];
        output_file<<",";}
    
    output_file<<huffman_codes.size();//size of huffman codes map
    output_file<<",";
    output_file<<padding;//number of padding bits
    output_file<<",";
   //writing the huffman codes
    for(auto&map:huffman_codes){
        
        output_file<<map.second;
        output_file<<",";
        output_file<<(char)map.first;
        output_file<<",";
    }
   //writing the compressed message
    for(int i =0;i<output_characters.size();i++){
     output_file<<output_characters[i];
     
    }

 
    output_file.close();
    //getting the size of the compressed file
    output_file_size= std::__fs::filesystem::file_size(path.c_str());
    
   
   
}
//main function for compression which call all other functions and generate the
//data strutures for them
void compress_file(char* file_location,char file_folder){
    string main_message;
    vector<int> file_message_lengths;
    priority_queue<pair<int, huffman_tree_node * >> input_priority_queue;
    vector<string> file_names;
    long long input_file_sizes,output_file_sizes;
    read_frequencies(input_priority_queue,main_message,file_message_lengths,file_location,file_folder,file_names,input_file_sizes);
    unordered_map<int, string> huffman_codes;
    HuffmanCodes(input_priority_queue, huffman_codes);
    output_compressed_file(huffman_codes,file_location,file_folder,main_message,file_message_lengths,file_names,output_file_sizes);
    //calculating the compression ratio
    double  compression_ratio = ((double)output_file_sizes/input_file_sizes)*100;

    cout<<"Compression ratio is: "<<compression_ratio<<"%"<<endl<<endl;


}
//function used to decompress the file
void decompress_file(string file_path,char file_folder){
    //opening the input compressed file and initializing all the variables to read from it
    ifstream input_file;
    input_file.open(file_path);
    int size;
    int padding;
    int no_of_files=0;
    
    char input_char;
    //////////////////////////////////////////////
    

    input_file>>no_of_files;//reading the number
    vector<string> file_names(no_of_files,"");
    vector<int> file_lengths(no_of_files);
    input_file.get(input_char);
    for(int i =0;i<no_of_files;i++){//reading the file lengths of all compresssed files "used in case of compressed folder decompression"
        input_file>>file_lengths[i];
        input_file.get(input_char);
    }
    for(int i =0;i<no_of_files;i++){//reading the names of the compressed files
        input_file>>input_char;
        while(input_char!=',')
        { file_names[i] += input_char;
        
            input_file>>input_char;
        }
            
    }
    input_file>>size;//reading the size of huffman tree
    input_file.get(input_char);
    input_file>>padding;//reading the compressed file padding
    input_file.get(input_char);
    unordered_map<string,char> map;
    while(size--){//reading the huffman codes hashmap
        char character;
        string code="";
        input_file.get(input_char);
        while(input_char!=','){
            code+=input_char;
            input_file.get(input_char);
        
        }
        input_file.get(character);
        input_file.get(input_char);
        map[code] =  character;
    }
    
    vector<bitset<8>> input;
   //reading the compressed string as bitsets of 8 bits
    while(input_file.get(input_char)){

        bitset<8> b(input_char);
        input.push_back(b);

    }
    input_file.close();
    input_file.clear();
   
    vector<bitset<1>> input_binary;
  //converting the vector of bitsets of 8 to vector of bitsets of 1
    for(auto &it:input){
    for(int i=7;i>=0;i--){
    
           bitset<1>temp(it[i]);
        input_binary.emplace_back(temp);
    }
    }
    //removing the padding bits
     while(padding--){
         input_binary.pop_back();

     }
    ofstream decompressed_file;
    string hash;
    int count=0;
    
    int j=0;
  //determining whether the output is a single file to same file or a compressed folder to multifiles
    if(file_folder=='2'){
        //if folder we open output file for first compressed file
        string root(file_path);
        root = root.substr(0,root.find_last_of("\\/"));
        cout<<(root+file_names[j] ).c_str()<<endl;
        decompressed_file.open((root+file_names[j] ).c_str() );
    }else  decompressed_file.open(file_path);
   //////////////////////////
    
//converting the binary input to its string equivelant bit by bit to check for
    //the equivelant character
    for(auto &it:input_binary){
        
       hash+=(it == 0)?"0":"1";
       if(map.find(hash)==map.end()){
           continue;
       } else{
           decompressed_file<<map[hash];
           hash="";
           count++;
           if(count == file_lengths[j]){
             
               j++;
               count=0;
               decompressed_file.close();
               string root(file_path);
               root = root.substr(0,root.find_last_of("\\/"));
             
               decompressed_file.clear();
               //if multifiles we continue opening files with names stored
               if(j<no_of_files){
                   cout<<(root+file_names[j] ).c_str()<<endl;
               decompressed_file.open((root+file_names[j] ).c_str() );
               }
           }
           
       }
    }
//    }
}
int main() {

   ///the main function contains the menu for the user and the running
    ///time of compression and decompression is messured here
    double start,end,time_taken;
    while(1){
    cout<<"Do you want to compress or decompress"<<endl;
    cout<<"Press 1 for compression and 2 for decompression and 3 to exit:  ";
    char choice = 0;
    cin>>choice;
        if(choice =='3'){
           break;
       }
    cout<<"press 1 for file and 2 for folder:"<<endl;
        char file_folder;
        cin>>file_folder;
       
    cout<<"please enter the file path: ";
    char file_path[255];
        ifstream test;
        while(1){
            file_path[0]='\0';
        cin>>file_path;
        cout<<endl;
        test.open(file_path);
            if(!test){
               cout<<"file doesn't exist please enter a correct path:"<<endl;
                test.close();
                test.clear();
            }else
                break;
        }
        test.close();
        test.clear();
    if(choice=='1'){
        start = clock();
        //calling the compression func. with directory specified by user
        //and with the choice of folder or file compression
        compress_file(file_path,file_folder);
         end = clock();
         time_taken = double(end - start) / double(CLOCKS_PER_SEC);
        cout<<"Compression Time = "<<time_taken<<" seconds"<<endl;
    }else if(choice == '2'){
        start = clock();
        //calling the compression func. with directory specified by user
        //and with the choice of folder or file compression
        decompress_file(file_path,file_folder);
         end = clock();
         time_taken = double(end - start) / double(CLOCKS_PER_SEC);
        cout<<"Decompression Time = "<<time_taken<<" seconds"<<endl;
    }
    }
    return 0;
}

