How to set up a new tuning algorithm:
-------------------------------------

The following instructions assume that you have successfully 
installed Qt and that you can compile and build the project.
Initially the Qt-IDE is closed and we are in the main folder
of the project. A new algorithm can be set up by carrying out
the following steps:

1. Think about a good name, e.g. "My new tuning method". 
   The corresponding files would be named 'mytuningmethod'.
   In the following instructions replace 'mytuningmethod' by
   the name of your algorithm.

2. Create a folder 'mynewtuningmethod' in the directory ./algorithm 
   and enter the folder.
   
		mkdir mynewtuningmethod
		cd mynewtuningmethod

3. Copy all files from the example algorithm into that folder

		cp ../examplealgorithm/* .
	
4. Rename all the files that you have copied

		rename examplealgorithm mynewtuningmethod *
		
5. Open mytuningmethod.pri in a text editor and replace all occurrences
   of examplealgorithm by mytuningmethod .
   
6. Similarly, open mytuningmethod.cpp in a text editor and replace
   in a case-sensitive way all occurrences of:
   
		examplealgorithm -> mynewtuningmethod
		ExampleAlgorithm -> MyNewTuningMethod
		EXAMPLEALGORITHM -> MYNEWTUNINGMETHOD

7. Repeat the same with mytuningmethod.h

8. Open mytuningmethod.xml and modify the data of the xml file.
   In particular modify the field <name>.
   This XML file controls the appearance of the algorithm in the GUI.
   The changes are mostly self-explaining.

9. Go back to the directory ./algorithm and open the file 
   algorithms.qrc in a text editor. Add a line:
   
           <file>mynewtuningmethod/mynewtuningmethod.xml</file>
   
10. Open the project in the Qt-IDE. You should see a new folder with
   the name mynewtuningmethod. When opening it you should see 4 entries,
   namely, mynewtuningmethod.pri, Headers, Sources, Other files.
   If this is not the case, please verify the previous step. 

11. Qmake, rebuild and start the project.
	Go to the tuning mode, press Info and select your new algorithm.
	Start the calculation. Your new algorithm should now render
	the mathematical equal temperament (a green horizontal line).
	Congratulations!
	
Now you can proceed to implement your own algorithm in the
files mynewtuningmethod.cpp and mynewtuningmethod.h.

END

    
   
	