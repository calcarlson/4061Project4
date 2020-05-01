# 4061Project4

Test Machine: csel-vole-37

Date: 04/30/20

Name: Cal Carlson, Nam Trinh, Jessica Cunningham

x.500: carl5362, trinh064, cunni536

# 1. The purpose

The purpose of our project is to create two seperate programs, a client and a server, which implement synchronization and inter process communication.

# 2. How to compile the program

To compile our program, use the provided makefile by using the make command in the terminal.

# 3. What our program does

Our program has two elements, the client and the server. The server receives file path arguments from the client and then creates threads based on the number of files within that file path and finds the total number of words for each letter of the alphabet and stores those statistics in an array which is returned to the client. The client is in charge of traversing the file structure and dividing the text files among the client processes.

# 4. Any assumptions outside the project 4 document

In addition to the assumptions outlined in the project 4 document, we assumed that only one of the provided testcases (testCase1 or testCase2) would be input into the program at a single time.

# 5. Team Members

Cal Carlson carl5362, 

Nam Trinh trinh064, 

Jessica Cunningham cunni536

# 6. Your and your partners individual contributions

Cal Carlson: Server and Readme

Nam Trinh: Client and Debugging -> File Traversal

Jessica Cunningham: Client and Debugging -> Generating Client Processes
