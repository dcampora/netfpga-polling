/* 
 * File:   Tools.h
 * Author: dcampora
 *
 * Created on August 23, 2012, 5:07 PM
 */

#include <string>
#include <sstream>

#ifndef TOOLS_H
#define	TOOLS_H

class Tools {
public:
    
    /** Returns a string with the contents of T t */
    template <class T>
    static std::string toString(T t){
        std::stringstream ss;
        std::string s;
        ss << t;
        ss >> s;
        return s;
    }
    
    
    /** Transforms a given number to binary format */
    template <class T>
    static std::string num2bin(T& a) {
        std::string s;
        char* cp = (char*)(&a); // How to do this? It can be const, so reinterpret won't work.
        for (int i=(sizeof(T)-1); i>=0; i--){
            for (int j=7; j>=0; j--){
                // LE
                int temp = (cp[i] >> j) & 1;
                s += Tools::toString<int>(temp);
                if (!(j % 4) && !(i==0 && j==0)) s += ' ';
            }
        }
        return s;
    }
    
    /** Prints a number of bits from the specified bitfield */
    template <class T>
    static std::string bitfield2bin(T& a, int num_bits_to_print) {
        std::string s;
        for (int i=num_bits_to_print - 1 /*(sizeof(T) * 8) - 1*/; i>=0; i--){
            int temp = (a >> i) & 1;
            s += Tools::toString<int>(temp);
            if (!(i % 4)) s += ' ';
        }
        return s;
    }
};



#endif	/* TOOLS_H */

