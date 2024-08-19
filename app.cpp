#define _CRT_SECURE_NO_WARNINGS
#include	<iostream>
#include 	<fstream>
#include	<string>
#include	<cstdlib>
#include	"List.h"
#include	"Student.h"
#include	"Exam.h"

using namespace std;

//compulsory functions --------------------------

bool CreateStuList(const char*, List*);				//Question 1
bool DeleteStudent(List*, char*);					//Question 2
bool DisplayStudent(List, int);						//Question 3
bool AddExamResult(const char*, List*);				//Question 4
bool recalculateResult(List, char*);				//Question 5
bool FilterStudent(List, List*, char*, int, int);	//Question 6
bool checkEligibleFYP(List, char*);					//Question 7
int menu();											//Question 8

//custom made functions --------------------------

int FindStudentIndex(List*, char*);		// used in Question 1 & 2
void r_print_exam(Exam);				// used in Question 5
void r_print_sub(Subject);				// used in Question 5
const char* r_getGrade(Subject);		// used in Question 5 & 7
double r_getGradePoint(Subject);		// used in Question 5



//compulsory functions --------------------------

int main() {
	List list1;
	int choice = 0;
	while (choice != 9)
	{
		char student_ID[12];
		int source;
		char course[3]; int year = 0, totalcredit = 0;
		List list2;
		
		choice = menu();//Question 8

		switch (choice)
		{
		case 1://Question 1
			if (CreateStuList("student.txt", &list1))
				cout << "Successfully read.\n";
			else
				cout << "Read file fail.\n";
			system("pause");
			break;

		case 2://Question 2
			cout << "Please enter student id for deletion: ";
			cin >> student_ID;
			if (DeleteStudent(&list1, student_ID))
				cout << "Successfully deleted.\n";
			else
				cout << "Deletion failed.\n";
			system("pause");
			break;

		case 3://Question 3
			cout << "Please enter the source to print (1 = screen, 2 = file): ";
			cin >> source;
			if (source != 1 && source != 2)
				cout << "Invalid input!\n";
			else
			{
				if (!DisplayStudent(list1, source))
					cout << "List is empty.\n";
			}
			system("pause");
			break;

		case 4://Question 4
			if (AddExamResult("exam.txt", &list1))
				cout << "Successfully added.\n";
			else
				cout << "Add exam failed.\n";
			system("pause");
			break;

		case 5://Question 5
			cout << "Enter the student id to recalculate result based on revised grading: ";
			cin >> student_ID;
			if (!recalculateResult(list1, student_ID))
				cout << "Cannot recalculate result.\n";
			system("pause");
			break;

		case 6://Question 6
			cout << "Please enter course to filter: ";
			cin >> course;
			cout << "Please enter year to filter: ";
			cin >> year;
			cout << "Please enter minimum total credit to filter: ";
			cin >> totalcredit;
			if (FilterStudent(list1, &list2, course, year, totalcredit))
			{
				if (!DisplayStudent(list2, 1))
					cout << "Filtered list is empty.\n";
			}
			else
			{
				cout << "List is empty.\n";
			}
			system("pause");
			break;

		case 7://Question 7

			cout << "Enter the student id to check if eligible to take FYP:";
			cin >> student_ID;
			if (!checkEligibleFYP(list1, student_ID))
				cout << "Cannot check FYP eligibility.\n";
			system("pause");
			break;

		default:
			break;
		}

	}

	cout << "Thank you for using the system.\n\n";

	return 0;
}

//Question 1
bool CreateStuList(const char* filename, List* list)
{
	//declare input file stream obj
	ifstream inpFile;

	//open the targeted file
	inpFile.open(filename);

	//check if fail in opening file 
	if (inpFile.fail()) {
		cout << "Open file failed, please make sure file is accessible.\n";
		return false;
	}

	Student student;
	string line;

	//read all from file and store as list
	while (!inpFile.eof())
	{
		getline(inpFile, line, '\n');//read input file line
		if (line != "")
		{
			strcpy(student.id, (line.substr(line.find("Student Id = ") + 13)).c_str());

			getline(inpFile, line, '\n');//read input file line
			strcpy(student.name, (line.substr(line.find("Name = ") + 7)).c_str());

			getline(inpFile, line, '\n');//read input file line
			strcpy(student.course, (line.substr(line.find("Course = ") + 9)).c_str());

			getline(inpFile, line, '\n');//read input file line
			strcpy(student.phone_no, (line.substr(line.find("Phone Number = ") + 15)).c_str());

			//check duplicate
			int index = FindStudentIndex(list, student.id);
			if (index < 1)
			{
				//cant find index, means not exist yet, free to insert
				list->insert(student);
			}
			else
			{
				cout << "Found existing student " << student.id << ", system will skip to prevent duplicate record.\n";
			}
		}
	}
	inpFile.close();

	//if list is empty that means student list failed to create
	if (list->empty())
		return false;
	return true;
}

//Question 2
bool DeleteStudent(List* list, char* id)
{
	//if list is empty at first, means deletion of student has failed
	if (list->empty())
		return false;

	int index = FindStudentIndex(list, id);
	if (index == 0)
		return false;

	//delete by index
	list->remove(index);
	return true;
}

//Question 3
bool DisplayStudent(List list, int source)
{
	if (list.empty())
		return false;

	if (source == 1)//screen
	{
		//traverse list to print
		int countStudent = 1;
		Node* cur;
		cur = list.head;
		while (cur != NULL)
		{
			cout << "\n******************************************************";
			cout << "STUDENT " << countStudent;
			cout << "******************************************************\n";

			cur->item.print(cout);//stream
			cout << "\n";

			if (cur->item.exam_cnt == 0)//no exam to print
			{
				cout << "THIS STUDENT HAVEN¡¯T TAKEN ANY EXAM YET\n";
			}
			else//got exam to print
			{
				cout << "--------------------------------------------------";
				cout << "PAST EXAM RESULT:--------------------------------------------------";

				//loop exams
				for (int countExam = 0; countExam < cur->item.exam_cnt; countExam++)
				{
					cur->item.exam[countExam].print(cout);//stream
				}

			}

			cout << "\n******************************************************";
			cout << "STUDENT " << countStudent;
			cout << "******************************************************\n\n";

			countStudent++;
			cur = cur->next;
		}

	}//source 1
	else if (source == 2)//file 
	{
		ofstream file;
		file.open("student_result.txt", std::ios_base::trunc); // overwrite instead of append
		if (file.fail())
		{
			cout << "Open file failed, please make sure file is accessible.\n";
			return false;
		}

		//traverse list to print
		int countStudent = 1;
		Node* cur;
		cur = list.head;
		while (cur != NULL)
		{
			file << "\n******************************************************";
			file << "STUDENT " << countStudent;
			file << "******************************************************\n";

			cur->item.print(file);//stream
			file << "\n";
			if (cur->item.exam_cnt == 0)
			{
				file << "THIS STUDENT HAVEN¡¯T TAKEN ANY EXAM YET\n";
			}
			else
			{
				file << "--------------------------------------------------";
				file << "PAST EXAM RESULT:--------------------------------------------------";

				//loop exams
				for (int countExam = 0; countExam < cur->item.exam_cnt; countExam++)
				{
					cur->item.exam[countExam].print(file);//stream
				}

			}//got exam to print

			file << "\n******************************************************";
			file << "STUDENT " << countStudent;
			file << "******************************************************\n\n";

			countStudent++;
			cur = cur->next;
		}
		//******************************************************
		file.close();
		cout << "Successfully printed!\n";

	}//source 2	
	return true;
}

//Question 4
bool AddExamResult(const char* filename, List* list)
{
	//declare input file stream obj
	ifstream inpFile;
	//open the targeted file
	inpFile.open(filename);
	//check if fail in opening file 
	if (inpFile.fail()) {
		cout << "Open file failed, please make sure file is accessible.\n";
		return false;
	}
	
	//read all from file and store as list
	while (!inpFile.eof())
	{
		//id	trimester 	year	numOfSubjects	
		//list of subjects (for every subject store unit code, unit name, credit hours and marks obtained)

		string str;
		inpFile >> str;//student id

		Exam e = Exam();
		inpFile >> e.trimester >> e.year;
		inpFile >> e.numOfSubjects;
		for (int i = 0; i < e.numOfSubjects; i++)
		{
			e.sub[i] = Subject();
			inpFile >> e.sub[i].subject_code;
			inpFile >> e.sub[i].subject_name;
			inpFile >> e.sub[i].credit_hours;
			inpFile >> e.sub[i].marks;
		}

		//now get student list to put exam data
		Node* cur;
		bool foundStudent = false;
		cur = list->head;
		while (cur != NULL)
		{
			if (strcmp(cur->item.id, str.c_str()) == 0) {
				foundStudent = true;
				break;
			}
			else {
				cur = cur->next;
			}
		}

		if (foundStudent)
		{
			//loop to find all exam of this student if it already has this exam record
			bool foundExam = false;
			for (int i = 0; i < cur->item.exam_cnt; i++)
			{
				if (cur->item.exam[i].year == e.year && cur->item.exam[i].trimester == e.trimester)
				{
					foundExam = true;
					break;
				}
			}
			if (foundExam)
			{
				//found the duplicate exam data, just ignore
				cout << "Found existing trimester exam (" << e.year << "-" << e.trimester << ") for student " << cur->item.id << ", system will skip to prevent duplicate record.\n";
			}
			else
			{
				//didnt find duplicate exam data of this student
				//so just assign exam data to student exam array
				cur->item.exam[cur->item.exam_cnt] = e;

				//calculate gpa for this exam
				cur->item.exam[cur->item.exam_cnt].calculateGPA();

				//increment of exam count
				cur->item.exam_cnt = cur->item.exam_cnt + 1;

				//calculate cgpa for student				
				cur->item.calculateCurrentCGPA();
			}
		}//found student
	}//read file
	inpFile.close();
	if (list->empty())
		return false;
	return true;
}

//Question 5
bool recalculateResult(List list, char* id)
{
	if (list.empty())
		return false;

	//now search student id
	Node* cur;
	bool foundStudent = false;
	cur = list.head;
	while (cur != NULL)
	{
		if (strcmp(cur->item.id, id) == 0) {
			foundStudent = true;
			break;
		}
		else {
			cur = cur->next;
		}
	}
	if (foundStudent)
	{
		Student stu = Student();
		stu = cur->item;

		//if no exam to print
		if (stu.exam_cnt == 0)
		{
			cout << "\nThis student have not taken any exam yet.\n";
			return false;
		}

		//has exam to print, use 3 steps to display recalculated results

		//step 1 - DISPLAY ORIGINAL
		cout << "\n\n\nRESULT OUTPUT BASED ON ORIGINAL GRADING:\n";
		cout << "_________________________________________";
		stu.print(cout);
		cout << "\n-----------------------------------------------ORIGINAL GRADING EXAM RESULT:-------------------------------------------------\n";
		//loop exams
		for (int countExam = 0; countExam < stu.exam_cnt; countExam++)
		{
			stu.exam[countExam].print(cout);
		}
		cout << "\n-----------------------------------------------ORIGINAL GRADING EXAM RESULT:-------------------------------------------------\n";

		//step 2 - REVISED
		//loop each exam to recalculate gpa with revised function
		for (int i = 0; i < stu.exam_cnt; i++)
		{
			double sum = 0;
			int total_credit_hours = 0;
			for (int j = 0; j < stu.exam[i].numOfSubjects; j++)
			{
				sum = sum + r_getGradePoint(stu.exam[i].sub[j]) * stu.exam[i].sub[j].credit_hours;
				total_credit_hours = total_credit_hours + stu.exam[i].sub[j].credit_hours;
			}
			stu.exam[i].gpa = sum / (double)total_credit_hours;
		}
		//recalculate student cgpa
		stu.calculateCurrentCGPA();


		//step 3 - DISPLAY REVISED
		cout << "\n\n\nRESULT OUTPUT BASED ON REVISED GRADING:\n";
		cout << "_________________________________________";
		stu.print(cout);		
		cout << "\n-----------------------------------------------REVISED GRADING EXAM RESULT:-------------------------------------------------\n";
		//loop exams
		for (int countExam = 0; countExam < stu.exam_cnt; countExam++)
		{
			r_print_exam(stu.exam[countExam]); // stu.exam[countExam].print(cout);
		}
		cout << "\n-----------------------------------------------REVISED GRADING EXAM RESULT:-------------------------------------------------\n";

		return true;
	}
	else
	{//student not found
		cout << "\nThere is no student with id " << id << " in the list.\n";
		return false;
	}
}

//Question 6
bool FilterStudent(List list1, List* list2, char* course, int year, int totalcredit)
{
	if (list1.empty() || !list2->empty())
		return false;

	//convert integer year to char array y1 & y2
	char y1[2], y2[2];
	_itoa((year % 100 / 10), y1, 10);//year first digit, 2021 : 2	
	_itoa((year % 10), y2, 10);//year last digit, 2021 : 1

	Node* cur;
	cur = list1.head;
	while (cur != NULL)
	{
		if (
			strcmp(cur->item.course, course) == 0 //compare course
			&& cur->item.totalCreditsEarned >= totalcredit //compare credit
			&& y1[0] == cur->item.id[0] //compare year first digit
			&& y2[0] == cur->item.id[1]//compare year last digit
			) {
			//add to list2
			list2->insert(cur->item);
		}
		cur = cur->next;
	}
	return true;
}

//Question 7
bool checkEligibleFYP(List list, char* id)
{
	if (list.empty())
		return false;

	//now search student id
	Node* cur;
	bool foundStudent = false;
	cur = list.head;
	while (cur != NULL)
	{
		if (strcmp(cur->item.id, id) == 0) {
			foundStudent = true;
			break;
		}
		else {
			cur = cur->next;
		}
	}
	if (foundStudent)
	{
		Student stu = Student();
		stu = cur->item;

		//if no exam
		if (stu.exam_cnt == 0)
		{
			cout << "\nThis student " << stu.name << " is not eligible to take FYP yet.\n";
			return false;
		}

		//has exams, use 2 steps to check eligibility for FYP

		//step 1 - check credit
		if (stu.totalCreditsEarned < 30)
		{
			cout << "\nThis student " << stu.name << " is not eligible to take FYP yet.\n";
			return false;
		}

		//step 2 - check pass result (grade C) for UCCD2502 and UCCD2513 
		int exam_UCCD2502, sub_UCCD2502; bool pass_UCCD2502 = false;
		int exam_UCCD2513, sub_UCCD2513; bool pass_UCCD2513 = false;
		for (int i = 0; i < stu.exam_cnt; i++)
		{
			for (int j = 0; j < stu.exam[i].numOfSubjects; j++)
			{
				//UCCD2502
				if (strcmp(stu.exam[i].sub[j].subject_code, "UCCD2502") == 0
					&& stu.exam[i].sub[j].marks >= 50		//C marks in original grading
					//&& stu.exam[i].sub[j].marks >= 56		//C marks in revised grading
					) {
					exam_UCCD2502 = i;
					sub_UCCD2502 = j;
					pass_UCCD2502 = true;
				}
				//UCCD2513
				if (strcmp(stu.exam[i].sub[j].subject_code, "UCCD2513") == 0
					&& stu.exam[i].sub[j].marks >= 50		//C marks in original grading
					//&& stu.exam[i].sub[j].marks >= 56		//C marks in revised grading
					) {
					exam_UCCD2513 = i;
					sub_UCCD2513 = j;
					pass_UCCD2513 = true;
				}
			}
		}
		if (pass_UCCD2502 && pass_UCCD2513)
		{
			cout << "\nThis student is eligible to take FYP. Below are the details of the students:";
			stu.print(cout);

			cout << "\nGrade obtained for UCCD2502 Introduction to Inventive Problem Solving is ";
			cout << stu.exam[exam_UCCD2502].sub[sub_UCCD2502].getGrade();		//original grading
			//cout << r_getGrade(stu.exam[exam_UCCD2502].sub[sub_UCCD2502]);	//revised grading

			cout << "\nGrade obtained for UCCD2513 Mini Project is ";
			cout << stu.exam[exam_UCCD2513].sub[sub_UCCD2513].getGrade();		//original grading
			//cout << r_getGrade(stu.exam[exam_UCCD2513].sub[sub_UCCD2513]);	//revised grading

			cout << "\n\n";
			return true;
		}
		else
		{
			cout << "\nThis student " << stu.name << " is not eligible to take FYP yet.\n";
			return false;
		}
	}
	else
	{//student not found
		cout << "\nThere is no student with id " << id << " in the list.\n";
		return false;
	}
}

//Question 8
int menu()
{
	system("cls");//clear screen
	int choice;
	cout << "FICT Student Management System\n";
	cout << "1. Create student list\n";
	cout << "2. Delete Student\n";
	cout << "3. Print student list\n";
	cout << "4. Add exam result\n";
	cout << "5. Recalculate result\n";
	cout << "6. Filter student\n";
	cout << "7. Check Eligible FYP Student\n";
	cout << "8. Exit.\n";
	cout << "\nPlease select your choice: ";
	//use do-while loop to make sure user choose 
	//a correct choice for menu number
	do
	{
		cin >> choice;
		fflush(stdin);//clear buffer to avoid skipping input bug
		if (choice < 1 || choice >8)
			cout << "Invalid choice, please try again: ";
	} while (choice < 1 || choice >8);
	return choice;
}



//custom made functions --------------------------


//this function can get student index by using id
//it helps to detect whether got existing stu id or not
//it also used in removing node from list by index
int FindStudentIndex(List* list, char* id)
{
	int index = 0;
	bool found = false;
	//find the index
	Node* cur;
	cur = list->head;
	while (cur != NULL)
	{
		index++;
		if (strcmp(cur->item.id, id) == 0) {
			found = true;
			break;
		}
		else {
			cur = cur->next;
		}
	}
	if (!found)
		return 0;
	else
		return index;
}


//this function print EXAM detail for REVISED GRADING
//it refer the original coding in Exam.cpp file
//the difference is that the print subject function uses REVISED version ===> r_print_sub
void r_print_exam(Exam e)
{
	cout << "\n\n" << e.printTrimester() << " " << e.year << " Exam Results: " << endl;

	cout << "\n" << e.numOfSubjects << " subjects taken.";
	cout << "\n___________________________________________________________________________________________________________________________";
	cout << "\nSubject Code\t" << setw(70) << left << "Subject Name" << "Credit Hours" << "\tGrade " << "\tGrade Point";
	cout << "\n___________________________________________________________________________________________________________________________";
	for (int i = 0; i < e.numOfSubjects; i++)
		r_print_sub(e.sub[i]);
	cout << "\nGPA: " << e.gpa;
	cout << "\n\n";
}


//this function print SUBJECT detail for REVISED GRADING
//it refer the original coding in Subject.cpp file
//the difference is that the getGrade & getGradePoint functions use REVISED version ===> r_getGrade and r_getGradePoint
void r_print_sub(Subject s)
{
	cout << "\n";
	cout << s.subject_code << "\t" << setw(70) << left << s.subject_name << setw(7) << right << s.credit_hours <<
		"\t" << setw(10) << left << " " << r_getGrade(s) << "\t  " << setprecision(5) << fixed << showpoint <<
		r_getGradePoint(s);
}


//this function determine subject grade for REVISED GRADING
//it refer the original coding in Subject.cpp file
//the difference is that the getGrade use REVISED GRADING in document ===> Figure2.doc
const char* r_getGrade(Subject s)
{
	if (s.marks >= 0 && s.marks < 36)
		return "F";
	else if (s.marks >= 36 && s.marks < 41)
		return "E";
	else if (s.marks >= 41 && s.marks < 46)
		return "D-";
	else if (s.marks >= 46 && s.marks < 56)
		return "D";
	else if (s.marks >= 56 && s.marks < 61)
		return "C";
	else if (s.marks >= 61 && s.marks < 66)
		return "C+";
	else if (s.marks >= 66 && s.marks < 71)
		return "B-";
	else if (s.marks >= 71 && s.marks < 76)
		return "B";
	else if (s.marks >= 76 && s.marks < 81)
		return "B+";
	else if (s.marks >= 81 && s.marks < 86)
		return "A-";
	else if (s.marks >= 86 && s.marks <= 100)
		return "A";
	else
		return "N/A";
}


//this function determine subject grade point for REVISED GRADING
//it refer the original coding in Subject.cpp file
//the difference is that the getGradePoint use REVISED GRADING in document ===> Figure2.doc
double r_getGradePoint(Subject s)
{
	if (s.marks >= 0 && s.marks < 36)
		return 0.00;
	else if (s.marks >= 36 && s.marks < 41)
		return 1.00;
	else if (s.marks >= 41 && s.marks < 46)
		return 1.33;
	else if (s.marks >= 46 && s.marks < 56)
		return 1.67;
	else if (s.marks >= 56 && s.marks < 61)
		return 2.00;
	else if (s.marks >= 61 && s.marks < 66)
		return 2.30;
	else if (s.marks >= 66 && s.marks < 71)
		return 2.70;
	else if (s.marks >= 71 && s.marks < 76)
		return 3.00;
	else if (s.marks >= 76 && s.marks < 81)
		return 3.30;
	else if (s.marks >= 81 && s.marks < 86)
		return 3.70;
	else if (s.marks >= 86 && s.marks <= 100)
		return 4.00;
	else
		return -1.0;
}