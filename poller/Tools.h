/* 
 * File:   Tools.h
 * Author: dcampora
 *
 * Created on August 23, 2012, 5:07 PM
 */

#ifndef TOOLS_H
#define	TOOLS_H

#include <string>
#include <sstream>
#include <vector>

#include <cstdlib>

#include <sys/stat.h>

using namespace std;

class Tools {
public:
    
    /** Returns a string with the contents of T t */
    template <class T>
    static string toString(T t){
        stringstream ss;
        string s;
        ss << t;
        ss >> s;
        return s;
    }
    
    
    /** Transforms a given number to binary format */
    template <class T>
    static string num2bin(T& a) {
        string s;
        char* cp = reinterpret_cast<char*>(&a);
        for (int i=(sizeof(T)-1); i>=0; i--){
            for (int j=7; j>=0; j--){
                // LE
                int temp = (cp[i] >> j) & 1;
                s += Tools::toString<int>(temp);
                if (!(j % 4) && !(i==(sizeof(T)-1) && j==7)) s += ' ';
            }
        }
        return s;
    }
    
    /** Prints a number of bits from the specified bitfield */
    template <class T>
    static string bitfield2bin(T& a, int num_bits_to_print) {
        string s;
        for (int i=num_bits_to_print - 1 /*(sizeof(T) * 8) - 1*/; i>=0; i--){
            int temp = (a >> i) & 1;
            s += Tools::toString<int>(temp);
            if (!(i % 4)) s += ' ';
        }
        return s;
    }
    
    /** Converts from vector<string> to argc, argv (returns argc, argv is a parameter) */
    static int vec2arg(vector<string>& v, char**& argv){
        int argc = v.size();
        argv = (char**) malloc((argc + 1) * sizeof(char*));
        for(int i=0; i<v.size(); i++){
            argv[i] = const_cast<char*>(v[i].c_str());
        }
        argv[argc] = NULL;
        
        return argc;
    }
    
    /** Converts from int32 to string */
    static string inttostr(int addr32){
        char* addrc = (char*)(&addr32);
        
        // TODO: test inet_ntoa
        
        return Tools::toString<int>( abs((int)addrc[0]) ) + "." +
               Tools::toString<int>( abs((int)addrc[1]) ) + "." +
               Tools::toString<int>( abs((int)addrc[2]) ) + "." +
               Tools::toString<int>( abs((int)addrc[3]) );
    }

    static bool fileExists(const string& filename){
        struct stat buf;
        if (stat(filename.c_str(), &buf) != -1)
            return true;
        return false;
    }
};

#endif	/* TOOLS_H */

