#include <dataTypes/types.hpp>
#include <iostream>
#include <ctime>
#include <memory>
#include <vector>
#include <algorithm>
#include <map>
#include <stack>

void removePoint(std::vector<std::uint32_t>& input, std::uint32_t id){
    for(unsigned int i = 0; i < input.size();i++){
        if(input[i] == id){
            input.erase(input.begin()+i);
            break;
        }
    }    
};

Figure grahamScan(Point* input, unsigned int maxSize){

    /////////////////////
    //INITIALIZING DATA//
    /////////////////////
    //Output, initial points and auxiliary points
    Figure output;
    Point* currentPoint = &input[0];
    //Fake point that is contigous to the first point
    Point fakePoint = Point();
    std::map<long double, Point*> orderedPoints;
    
    //////////////////////////////
    //LOOKING FOR THE BEST POINT//
    //////////////////////////////


    //Looking for the lowest Y value
    for(unsigned int i = 0; i < maxSize;i++){
        Point* auxPoint = &input[i];
        if(auxPoint->Y_Position < currentPoint->Y_Position){
            currentPoint = auxPoint;
        }
    }

    
    //We put the fake point in a contiguous position to the initial point
    fakePoint.Y_Position = currentPoint->Y_Position;
    fakePoint.X_Position = currentPoint->X_Position-5.0f;


    ///////////////////////////////////
    //SORT ALL OF THE POINTS BY ANGLE//
    ///////////////////////////////////


    //For every point in the set, we get the angle between 
    //fake point and the next point and order it in increasing order
    for(unsigned int i = 0; i <= maxSize;i++){
        Point* auxPoint = &input[i];
        if(auxPoint != currentPoint){
            Angle toTest = Angle(currentPoint,&fakePoint, auxPoint);
            double long degrees = toTest.degrees;
            if(degrees <= 0.0f)
                degrees += 360.0f;
            orderedPoints.emplace(degrees,auxPoint);
        }
    }

    /////////////
    //ALGORITHM//
    /////////////
    //Emplace the first point in the stack, which is the lowest point in Y
    output.allPoints.push_back(*currentPoint);

    //For each other point
    for (auto iter = orderedPoints.rbegin(); iter != orderedPoints.rend(); ++iter) {
        bool validPoint = false;
        //if the stack of points is bigger than 2
        if(output.allPoints.size() >= 2){
            //while our point is not valid yet
            while(!validPoint){
                //Take the angle between the last point in the stack, the second last point and the new point
                Angle toCheck = Angle(&output.allPoints[output.allPoints.size()-2],
                &output.allPoints[output.allPoints.size()-1],iter->second);
                //If the angle is obtuse it means it did a clock-wise turn
                if(toCheck.degrees < 0.0f){
                    //delete the last point in the stack
                    output.allPoints.pop_back();
                    //if there's only the first point in the stack, we add the new point to the stack
                    if(output.allPoints.size() == 1){
                        validPoint = true;
                        output.allPoints.push_back(*iter->second);
                    }
                }
                //If the angle is an acute angle it means is convex
                //we add it to the stack
                else{
                    output.allPoints.push_back(*iter->second);
                    validPoint = true;
                }
            }
        }
        //If there's only one point in the stack, we add the next one
        else{
            output.allPoints.emplace_back(*iter->second);
        }
    }

    ////////////////////////////////
    //ADD EVERYTHING TO THE FIGURE//
    ////////////////////////////////

    for(unsigned int i = 0; i < output.allPoints.size();i++){
        if(i != output.allPoints.size()-1){
            output.allLines.emplace_back(output.allPoints[i].id,output.allPoints[i+1].id);
        }
        else{
            output.allLines.emplace_back(output.allPoints[i].id,output.allPoints[0].id);
        }
    }

    /*for(unsigned int i = 0; i < output.allPoints.size();i++){
        std::cout << "Point at " << i << " is " << output.allPoints[i].id << std::endl;
    }
    for(unsigned int i = 0; i < output.allLines.size();i++){
        std::cout << "Line at " << i << " connected to " << output.allLines[i].idFirst << " and " << output.allLines[i].idSecond << std::endl;
    }*/

    return output;

}

Figure packageWrap(Point* input, unsigned int maxSize){

    /////////////////////
    //INITIALIZING DATA//
    /////////////////////

    //Output, initial points and auxiliary points
    Figure output;
    //std::uint32_t initialPoint = input[0].id;
    std::uint32_t initialPoint = input[0].id;
    Point* currentPoint = &input[0];
    Point* previousPoint = nullptr;
    std::vector<std::uint32_t> possiblePoints;
    //Fake point that is contigous to the first point
    Point fakePoint = Point();
    
    //////////////////////////////
    //LOOKING FOR THE BEST POINT//
    //////////////////////////////

    //Looking for the lowest Y value
    for(unsigned int i = 1; i < maxSize;i++){
        Point* auxPoint = &input[i];
        possiblePoints.push_back(auxPoint->id);
       
        if(auxPoint->Y_Position < currentPoint->Y_Position){
            currentPoint = auxPoint;
            initialPoint = currentPoint->id;
        }
    }

    //Move the fake point into a contigous place of the current Point
    fakePoint.Y_Position = currentPoint->Y_Position;
    fakePoint.X_Position = currentPoint->X_Position-5.0f;
    previousPoint = &fakePoint;


    /////////////
    //ALGORITHM//
    /////////////
    
    
    //IF the ID of the current point is not the ID of the final point
    do{
        //the current degree is 0
        long double currentDegrees = 0.0f;
        //the possible next point ID, initially 0
        std::uint32_t possibleNextPoint = 0;
        //for each possible point
        for(unsigned int i = 0; i < possiblePoints.size(); i++){
            //we get the reference to the input
            Point* auxPoint = &input[possiblePoints[i]]; 
            //We create an angle and calculate the degrees
            Angle toTest = Angle(currentPoint,previousPoint,auxPoint);
            
            if(toTest.degrees < 0.0f){
                toTest.degrees += 360.0f;
            }

            //If the angle is WIDER
            if(currentDegrees < toTest.degrees){
                //Then this is the possible next point for wrapping
                currentDegrees = toTest.degrees;
                possibleNextPoint = auxPoint->id;
            }
        }
        //current point becomes previous
        previousPoint = currentPoint;
        //next point becomes next point 
        currentPoint = &input[possibleNextPoint];
        //we remove current point from the list
        removePoint(possiblePoints,currentPoint->id);
        //We add the new point to the solution
        output.allPoints.push_back(*currentPoint);
        //And we create a new line for the solution
        output.allLines.push_back({previousPoint->id,currentPoint->id});
    }while(currentPoint->id != initialPoint);

    /*for(unsigned int i = 0; i < output.allLines.size();i++){
        std::cout << "Line number: " << i << "\nFirst Point: " <<output.allLines[i].idFirst 
            << ", Second Point: " <<output.allLines[i].idSecond << std::endl;
    }*/

    return output;
}

void test(){

    Point one, two, three;
    one.X_Position = 1;
    one.Y_Position = 0.1;
    two.X_Position = 0;
    two.Y_Position = 0;
    three.X_Position = 0;
    three.Y_Position = 1;
    {
        Angle test{&two,&one,&three};
        std::cout << "Angle: " << test.degrees << std::endl;
    }
}

int main(int argc, char *argv[]){
    //Solution to the algorithm
    Figure solution;
    //Smart pointer to the points of the convex hull
    std::unique_ptr<Point[]> points;
    //Max number of points
    unsigned int numberOfPoints;
    if(argc < 2){
        std::cout << "Input the number of points to be included in the figure.\n";
        std::cin >> numberOfPoints;
    }
    else{
        numberOfPoints = atoi(argv[1]);
    }

    //Reserve memory to the Heap in order to store Points
    points = std::make_unique<Point[]>(numberOfPoints);

    //Create pseudo-random seed
    srand((unsigned int)time(nullptr));
    //Per each point, create random position in X and Y
    for(unsigned int i = 0; i < numberOfPoints; i++){
        Point* PointToCreate = &points[i];
        PointToCreate->X_Position = 10000*static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
        PointToCreate->Y_Position = 10000*static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
        //std::cout << "Point id: " << PointToCreate->id << "\n";
        //std::cout << "Position X: " << PointToCreate->X_Position << "\nPosition Y: " << PointToCreate->Y_Position <<"\n";
    }

    {
        int option;
        if(argc < 2){
            std::cout << "Select one algorithm: \n(1) Package Wrap\n(2) Graham Scan\n";
            std::cin >> option;
        }
        else{
            option = atoi(argv[2]);
        }
        
        switch (option){
            case 1:
                //std::cout << "Proceeding to Package Wrap\n";
                solution = packageWrap(points.get(), numberOfPoints);
            break;
            case 2:
                solution = grahamScan(points.get(), numberOfPoints);
            break;
            case 3:
                test();
            break;
            default:
                std::cout << "Wrong option\n";
            break;
        }
    }

    return 0;
}