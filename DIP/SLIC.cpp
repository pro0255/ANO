#include "stdafx.h"
#include "SLIC.h"

SLIC::SLIC()
{
}

SLIC::~SLIC()
{
}

//http://mrl.cs.vsb.cz/people/gaura/ano/slic.pdf


void SLIC::run(cv::Mat image, int epochs, int k)
{

	cv::Mat output = image.clone();

	std::vector<Point> points = this->createPoints(image);
	std::vector<Point> centroids = this->initCentroids(image);


	for (std::vector<Point>::iterator c = begin(centroids); c != end(centroids); ++c)
	{
		int clusterId = c - begin(centroids);

		for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
		{
			Point p = *it;
			double dist = c->distance(p);

			if (dist < p.minDist)
			{
				p.minDist = dist;
				p.cluster = clusterId;
			}
			*it = p;
		}
	}

	for (int e = 0; e < epochs; e++)
	{
		cout << "Epoch " << e << endl;
		this->drawCentroids(centroids, image, e);

		vector<vector<int>> sumR;
		vector<vector<int>> sumG;
		vector<vector<int>> sumB;
		vector<vector<int>> sumX;
		vector<vector<int>> sumY;


		for (int j = 0; j < k; ++j) {
			sumR.push_back(vector<int> {});
			sumG.push_back(vector<int> {});
			sumB.push_back(vector<int> {});
			sumX.push_back(vector<int> {});
			sumY.push_back(vector<int> {});
		}


		//set points
		for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
		{
			int myIndex = it->cluster;
			sumR.at(myIndex).push_back(it->r);
			sumG.at(myIndex).push_back(it->g);
			sumB.at(myIndex).push_back(it->b);
			sumX.at(myIndex).push_back(it->x);
			sumY.at(myIndex).push_back(it->y);
		}

		//recalculate

		for (std::vector<Point>::iterator c = centroids.begin(); c != centroids.end(); ++c)
		{

			int clusterId = c - begin(centroids);

			auto clustersumX = sumX.at(clusterId);
			auto clustersumY = sumY.at(clusterId);
			auto clustersumR = sumR.at(clusterId);
			auto clustersumG = sumG.at(clusterId);
			auto clustersumB = sumB.at(clusterId);

			Point p = *c;
			p.x = std::accumulate(clustersumX.begin(), clustersumX.end(), 0) / clustersumX.size();
			p.y = std::accumulate(clustersumY.begin(), clustersumY.end(), 0) / clustersumY.size();
			p.r = std::accumulate(clustersumR.begin(), clustersumR.end(), 0) / clustersumR.size();
			p.g = std::accumulate(clustersumG.begin(), clustersumG.end(), 0) / clustersumG.size();
			p.b = std::accumulate(clustersumB.begin(), clustersumB.end(), 0) / clustersumB.size();
			*c = p;
		}


		for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
		{
			it->minDist = DBL_MAX;
		}


		//can be optimalized according to 2S x 2S? no time :}
		for (std::vector<Point>::iterator c = begin(centroids); c != end(centroids); ++c)
		{
			int clusterId = c - begin(centroids);

			for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
			{
				Point p = *it;
				double dist = c->distance(p);

				if (dist < p.minDist)
				{
					p.minDist = dist;
					p.cluster = clusterId;
				}
				*it = p;
			}
		}
	}

	for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it) //Projde všechny body
	{
		for (std::vector<Point>::iterator c = centroids.begin(); c != centroids.end(); ++c) //Pøes všechny centroidy
		{
			int clusterId = c - begin(centroids); //Zjištìní id pro cluster

			if (clusterId == it->cluster) //Tenhle bod má tenhle cluster pak vykreslím na tomto bodì tuto barvu
			{
				output.at<cv::Vec3b>(it->x, it->y) = cv::Vec3b(c->b, c->g, c->r);
			}
		}
	}

	cv::namedWindow("Output", cv::WINDOW_NORMAL);
	imshow("Output", output);
}




void SLIC::drawCentroids(vector<Point> centroids, cv::Mat & input, int epoch)
{
	cv::Mat  drawCentroidsImg = input.clone();

	for (int i = 0; i < centroids.size(); i++)
	{
		int y = centroids.at(i).y;
		int x = centroids.at(i).x;
		cv::circle(drawCentroidsImg, cv::Point(x, y), 3, CV_RGB(255, 0, 0), CV_FILLED);
	}

	string name = "CentroidImage" + to_string(epoch);

	cv::imshow(name, drawCentroidsImg);

}

vector<Point> SLIC::createPoints(cv::Mat & image)
{

	std::vector<Point> points;

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			cv::Vec3b bgr = image.at<cv::Vec3b>(i, j);
			points.push_back(Point(Point(bgr[2], bgr[1], bgr[0], i, j)));
		}
	}

	return points;
}

vector<Point> SLIC::initCentroids(cv::Mat & input)
{

	vector<Point> centroids;

	float interval_x = input.cols / 5;
	float interval_y = input.rows / 3;

	cv::Vec3b color = cv::Vec3b(0, 0, 255);

	for (int i = 0; i < k; ++i)
	{
		int x = interval_x / 2 + (i % 5) * interval_x;
		int y = interval_y / 2 + (i % 3)* interval_y;

		cv::Vec3b bgr = input.at<cv::Vec3b>(y, x);
		centroids.push_back(Point(bgr[2], bgr[1], bgr[0], x, y));

		input.at<cv::Vec3b>(y, x) = color;
	}

	return centroids;
}

