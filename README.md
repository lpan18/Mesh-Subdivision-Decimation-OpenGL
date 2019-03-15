# CMPT764 Assignment2

## Mesh Decimation
unzip file      
run ```cmake . ```          
run ```make```         
run ```./mcaq```          
Click open button to open an obj file.      
Enter the k value and Number of vertices you want to collaspe in the textbox, and then click button "Decimate".        
Click save button to save the decimated mesh to an obj file.           
Click quit button to quit the program.          

## Structure of code files        
The MeshMcd.h and MeshMcd.cpp contains the main structure of mesh decimation algorithm.     
The MeshSd.h and MeshSd.cpp contains the main structure of mesh subdivision algorithm.     
The W_edge.h, W_edge.cpp, Mesh.h, Mesh.cpp contains the main structure of winged edge and mesh structure.           
The obj_view.cpp contains the user interface.            
