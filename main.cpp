#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;
vector<string> split(string str, char pattern, bool enter);
vector<vector<string>> enlace;
vector<vector<string>> modo;
vector<vector<string>> proto;
vector<vector<string>> usuario;
vector<vector<string>> macusuario;


void abrir();
void create0();
void create1();
void create2();
void create3();
void modalidad1();
void modalidad2();

FILE * archivo;


long medida;
char * textoenlace;
char * textomodo;
char * textoproto;
char * textousuario;

int counter;
int id;
int upband=0;
int downband=0;
int main()
{
    id=0;
    counter=0;
    create0();
    create1();
    create2();
    modalidad1();
    return 0;
}

//crear array de enlace
void create0(){
    abrir();
    vector<string> res=split(textoenlace,'\n',false);
    for(int i=0; i<res.size(); i++){
        enlace.push_back(split(res[i],'=',false));
    }

    for(int i=0; i<enlace.size(); i++){
        vector<string>tmp=enlace[i];
        if(tmp[0]=="up"){
            upband=stoi(tmp[1]);
        }else if(tmp[0]=="down"){
            downband=stoi(tmp[1]);
        }
    }
}

//crear array de modo
void create1(){
    abrir();
    vector<string> modo=split(textomodo,'=',false);

}

//crear array de usuario
void create2(){
    abrir();
    vector<string> res=split(textousuario,'\n',false);
    for(int i=0; i<res.size(); i++){

        usuario.push_back(split(res[i],',',false));
    }

    /*for(int i=0; i<usuario.size(); i++){
        vector<string>tmp=usuario[i];
        //vector<string>mac=split(tmp[0],':',false);
        for(int k=0;k<mac.size();k++){
                cout<<mac[k];
        }
        for (int j=1; j<tmp.size();j++){

            cout<<tmp[j]<<"**"<<endl;
        }
    }*/

}

//crear array de protocolo
void create3(){
    abrir();
    vector<string> res=split(textoproto,',',false);
    for(int i=0; i<res.size(); i++){
        proto.push_back(split(res[i],'=',false));
    }

    for(int i=0; i<proto.size(); i++){
        vector<string>tmp=proto[i];
        cout<<tmp[0]<<endl;
    }

}

//abrir archivos y almacenarlos
void abrir(){

    if (counter==0){
        archivo = fopen ("enlace.conf", "r");
    }
    if (counter==1){
        archivo= fopen ("modo.conf", "r");
    }
    if (counter==2){
        archivo = fopen ("usuario_BW.conf", "r");
    }

    if (counter==3){
        archivo= fopen ("usuario_Proto.conf", "r");
    }

    fseek (archivo , 0 , SEEK_END);
    medida = ftell (archivo);
    rewind (archivo);
    if (counter==0){
        textoenlace = (char*) malloc (sizeof(char)*medida);
        fread(textoenlace, medida+1, 1, archivo);

    }
    if (counter==1){
        textomodo = (char*) malloc (sizeof(char)*medida);
        fread(textomodo, medida+1, 1, archivo);

    }
    if (counter==2){
        textousuario = (char*) malloc (sizeof(char)*medida);
        fread(textousuario, medida+1, 1, archivo);

    }

    if (counter==3){
        textoproto = (char*) malloc (sizeof(char)*medida);
        fread(textoproto, medida+1, 1, archivo);

    }

    fclose(archivo);
    counter=counter+1;
}

vector<string> split(string str, char pattern,bool enter) {
    int posInit = 0;
    int posFound = 0;
    string splitted;
    vector<string> resultados;
    char chars[]="(\n\t";
    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;

        if(enter){
            for (unsigned int i = 0; i < 2; ++i) {
            splitted.erase (std::remove(splitted.begin(), splitted.end(), chars[i]), splitted.end());
            }
        }

        resultados.push_back(splitted);

    }


    return resultados;
}


void modalidad1(){

    //system("tc qdisc del dev interfaz root ");
    //system("insmod sch_htb 2> /dev/null ");
    //system("tc qdisc add dev $DEV root       handle 1:    htb default 0xA");

    for(int i=0; i<usuario.size(); i++){
        vector<string>tmp=usuario[i];
        vector<string>mac=split(tmp[0],':',false);
        string command="tc class add dev enp7s0 parent 1:1 classid 1:"+to_string(i+1);
        string strdown=tmp[1];
        string strup=tmp[2];
        int intup=(stoi(strup)*10)*upband;
        int intdown=(stoi(strdown)*10)*downband;
        cout<<intup<<"->"<<intdown<<endl;
        command=command+" htb rate "+to_string(intdown)+"kbit";
        string M2= mac[4]+mac[5];
        string M0= mac[0]+mac[1];
        string M1= mac[2]+mac[3];

        string filter_by_mac1="tc match u16 0x"+M2+" 0xFFFF at -4 match u32 0x"+M0+M1;
        filter_by_mac1=filter_by_mac1+"  0xFFFFFFFF at -8 flowid 1:"+to_string(i+1);

        string filter_by_mac2="tc match u32 0x"+M1+M2+" 0xFFFFFFFF at -12 match u16 0x"+M0;
        filter_by_mac2=filter_by_mac2+" 0xFFFF at -14 flowid 1:"+to_string(i+1);
        //system(filter_by_mac1.c_str());
        //system(filter_by_mac2.c_str());
    }
}
