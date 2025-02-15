/*
This kmeans clustering algorithm was implemented using this tutorial:
https://reasonabledeviations.com/2019/10/02/k-means-in-cpp/. Modifications to
that code include the addition of a third dimension as well as reading and
parsing a different dataset.
*/

#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "csv.hpp"

using namespace std;

struct Point {
  double x, y, z; // coordinates
  int cluster;    // no default cluster
  double minDist; // default infinite dist to nearest cluster

  Point() : x(0.0), y(0.0), z(0.0), cluster(-1), minDist(__DBL_MAX__) {}

  Point(double x, double y, double z)
      : x(x), y(y), z(z), cluster(-1), minDist(__DBL_MAX__) {}

  double distance(Point p) {
    return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y) +
           (p.z - z) * (p.z - z);
  }
};

vector<Point> readcsv(std::string cat1, std::string cat2, std::string cat3) {
  vector<Point> points;
  std::vector<Point> currentLine;
  csv::CSVReader reader("tracks_features.csv");
  for (csv::CSVRow &row : reader) {
    points.push_back(Point(row[cat1].get<double>(), row[cat2].get<double>(),
                           row[cat3].get<double>()));
  }
  return points;
}

void kMeansClustering(int epochs, int k, std::string category1,
                      std::string category2, std::string category3) {
  vector<Point> points = readcsv(category1, category2, category3); // read from file

  vector<Point> centroids;
  srand(time(0)); // need to set the random seed
  for (int i = 0; i < k; ++i) {
    centroids.push_back(points.at(rand() % points.size()));
  }

  for (vector<Point>::iterator c = begin(centroids); c != end(centroids); ++c) {
    // quick hack to get cluster index
    int clusterId = c - begin(centroids);

    for (vector<Point>::iterator it = points.begin(); it != points.end();
         ++it) {

      Point p = *it;
      double dist = c->distance(p);
      if (dist < p.minDist) {
        p.minDist = dist;
        p.cluster = clusterId;
      }
      *it = p;
    }
  }

  vector<int> nPoints;
  vector<double> sumX, sumY, sumZ;

  // Initialize with zeroes
  for (int j = 0; j < k; ++j) {
    nPoints.push_back(0);
    sumX.push_back(0.0);
    sumY.push_back(0.0);
    sumZ.push_back(0.0);
  }

  // Iterate over points to append data to centroids
  for (vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
    int clusterId = it->cluster;
    nPoints[clusterId] += 1;
    sumX[clusterId] += it->x;
    sumY[clusterId] += it->y;
    sumZ[clusterId] += it->z;

    it->minDist = __DBL_MAX__; // reset distance
  }

  // Compute the new centroids
  for (vector<Point>::iterator c = begin(centroids); c != end(centroids); ++c) {
    int clusterId = c - begin(centroids);
    c->x = sumX[clusterId] / nPoints[clusterId];
    c->y = sumY[clusterId] / nPoints[clusterId];
    c->z = sumZ[clusterId] / nPoints[clusterId];
  }

  ofstream myfile;
  myfile.open("tracks_output.csv");
  myfile << category1 << "," << category2 << "," << category3 << ",c" << endl;

  for (vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
    myfile << it->x << "," << it->y << "," << it->z << "," << it->cluster
           << endl;
  }
  myfile.close();
}

int main(int argv, char *argc[]) {
  int numEpochs = 10;
  int k = 3;
  kMeansClustering(numEpochs, k, "danceability", "loudness",
                   "instrumentalness");
  return 0;
}
