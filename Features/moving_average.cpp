#include <queue>
#include <SPI.h>
//using namespace std;

double SimpleMovingAverage(input_data) {
    queue<double> dataset;
    
    // 3 point moving average
    int period = 3;
    double sum;
    double mean;

    //assuming that input data will be an array of doubles
    for(double x : input_data) {
        sum += x;
        dataset.push(x);

        if(dataset.size() > period) {
            sum -= dataset.front();
            dataset.pop();
        }

        mean = sum / period;
        Serial.print("New mean value is ")
        Serial.println(mean)
        
        return mean;
    }
}

  
