/*
 *  Alpha emulation library
 *  Copyright (C) 2014 Franklin Wei
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <iostream>
#include <alpha.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <cctype>
#include <cstdio>
#include <csignal>
using namespace std;
char hex_chars[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
alpha_ctx* ctx;

void run()
{
  while(!ctx->done)
    alpha_disasm(ctx);
}
int main(int argc, char* argv[])
{
  istream* in=&cin;
  ifstream ifs;
  if(argc>=2)
    {
      ifs.open(argv[1]);
      // read from file
      if(ifs.good())
	in=&ifs;
      else
	{
	  cerr << "Cannot read from file.\n" << endl;
	  return -1;
	}
    }
  bool interactive=false;
  if(in==&cin)
    interactive=true;
  vector<byte> prog;
  while(in->good())
    {
      if(interactive)
	printf("0x%08X:", (unsigned int)prog.size()); // really shouldn't mix C-style and stream I/O!
      string line;
      getline(*in, line); // get line to comment
      for(unsigned int i=0;i<line.length();i+=2)
	{
	  byte val=0xFF; 
	  for(int j=0;j<16;++j)
	    {
	      if(toupper(line[i+1])==hex_chars[j])
		{
		  val=j;
		}
	    }
	  for(int j=0;j<16;++j)
	    {
	      if(toupper(line[i])==hex_chars[j])
		{
		  val|=((j&0xF)<<4);
		}
	    }
	  prog.push_back(val);
	}
    }
  byte* p=(byte*)malloc(prog.size());
  for(unsigned int i=0;i<prog.size();++i)
    p[i]=prog[i];
  ctx=alpha_init((byte*)p, // memory
		 prog.size(), // mem size
		 0, // stack size
		 prog.size()); // initial stack pointer
  run();
  return ctx->return_value;
}
