#include "stdafx.h"
#include "SLIC.h"

SLIC::SLIC()
{
}

SLIC::~SLIC()
{
}

//http://mrl.cs.vsb.cz/people/gaura/ano/slic.pdf

//TODO


cv::Mat & SLIC::run(unsigned int K, cv::Mat & input, float threshold, float m, int epochs)
{
	cv::Mat output = input.clone();
	const int N = input.rows * input.cols;
	const float APPROX_S = N / K;
	const float S = sqrt(APPROX_S);
	cout << "N -> " << N << endl;
	cout << "APPROX_S -> " << APPROX_S << endl;
	cout << "S -> " << S << endl;
	int counter = 0;
	vector<vector<float>> centroids = init(S, K, output);
	//lowest gradient position in a 3 × 3 neighborhood TODO!


	//CLUSTERING
	while (true) {
		vector<vector<vector<float>>> centroid2pixels;



		cout << "\nStarting epoch " << counter << endl;

		//Init vectores
		for (int i = 0; i < K; i++)
		{
			centroid2pixels.push_back(vector<vector<float>> {});
		}

		const int area = 2 * S;

		for (int y = 0; y < input.rows; y++)
		{
			for (int x = 0; x < input.cols; x++)
			{
				//cout << y << " " << x << endl;
				cv::Vec3b pixel = input.at<cv::Vec3b>(y, x);
				//cout << pixel << endl;
				vector<float> pixelVector{ float(pixel[0]), float(pixel[1]) ,float(pixel[2]), float(x), float(y) };

				//Find possible centroids
				vector<float> possibleCentroids;
				for (int i = 0; i < centroids.size(); i++)
				{
					const int c_x = centroids.at(i).at(3);
					const int c_y = centroids.at(i).at(4);

					const int d_x = abs(c_x - x);
					const int d_y = abs(c_y - y);


					if (d_x <= area && d_y <= area) {
						possibleCentroids.push_back(i); //centroid in area;
					}
				}

				//Find closest
				int min_index = -1;
				float min_distance = -1;
				for (int i = 0; i < possibleCentroids.size(); i++)
				{
					const int centroidIndex = possibleCentroids.at(i);
					const auto centroidVector = centroids.at(centroidIndex);



					auto d = this->distance(pixelVector, centroidVector, m, S);
					if (min_index == -1 || d < min_distance) {
						min_index = centroidIndex;
						min_distance = d;
					}

				}

				centroid2pixels.at(min_index).push_back(pixelVector);
			}
		}


		//Update centroids
		vector<vector<float>> new_centroids;
		for (int i = 0; i < K; i++) //Pøes centroidy
		{
			new_centroids.push_back(vector<float> {});
			for (int j = 0; j < centroids.at(0).size(); j++) //Pøes pixely centroidù
			{
				float sum = 0;
				for (int m = 0; m < centroid2pixels.at(i).size(); m++) //Pøes danou feature
				{
					auto v = centroid2pixels.at(i).at(m).at(j);
					sum += v;
				}
				auto mean = sum / float(centroid2pixels.at(i).size());
				new_centroids.at(i).push_back(mean);
			}
		}



		//for (int i = 0; i < centroids.size(); i++)
		//{
		//	cout << centroid_objects.at(i).size() << endl;
		//}
		//cout << endl;

		for (int i = 0; i < centroids.size(); i++)
		{
			cout << "Old centroid " << i << endl;
			for (int j = 0; j < centroids.at(i).size(); j++)
			{
				cout << centroids.at(i).at(j) << ';';
			}
			cout << endl;
		}


		for (int i = 0; i < new_centroids.size(); i++)
		{
			cout << "New centroid " << i << endl;
			for (int j = 0; j < new_centroids.at(i).size(); j++)
			{
				cout << new_centroids.at(i).at(j) << ';';
			}
			cout << endl;

		}






		bool isBreaking = false;
		for (int i = 0; i < new_centroids.size(); i++)
		{
			if (new_centroids.at(i) == centroids.at(i) || this->distance(new_centroids.at(i), centroids.at(i), m, S) < threshold) {
				//cout << "same" << endl;
				isBreaking = true;
			}
			else {
				//cout << "different" << endl;
				isBreaking = false;
				break;
			}
		}

		if (!isBreaking && counter > epochs) {
			isBreaking = true;
		}

		centroids = new_centroids;


		//Draws positions

		cv::Mat drawCentroids = input.clone();
		this->drawPoints(centroids, drawCentroids);
		cv::imshow("Image", drawCentroids);

		if (isBreaking) {
			break;
		}
		cout << "\EEnding epoch " << counter << endl;
		counter++;



	}
	/*

		map<int, vector<float>> label_2_ethalon;
		for (int i = 0; i < centroids.size(); i++)
		{
			label_2_ethalon.insert(pair<int, vector<float>>(i, centroids.at(i)));
		}

		this->label_2_ethalon = label_2_ethalon;



	*/
	//cv::Mat drawCentroids = input.clone();
	//this->drawPoints(centroids, drawCentroids);



	cv::Mat drawCentroids = input.clone();
	this->drawPoints(centroids, drawCentroids);

	cv::imshow("Image", drawCentroids);


	return input;
}

void SLIC::run2(cv::Mat image, int epochs, int k)
{
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

		for (int j = 0; j < k; ++j)
		{
			std::vector<int> sumR;
			std::vector<int> sumG;
			std::vector<int> sumB;
			std::vector<int> sumX;
			std::vector<int> sumY;

			for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
			{
				if (it->cluster == j)
				{
					sumR.push_back(it->r);
					sumG.push_back(it->g);
					sumB.push_back(it->b);
					sumX.push_back(it->x);
					sumY.push_back(it->y);
				}
			}

			for (std::vector<Point>::iterator c = centroids.begin(); c != centroids.end(); ++c)
			{
				int clusterId = c - begin(centroids);

				if (clusterId == j)
				{
					Point p = *c;
					p.x = std::accumulate(sumX.begin(), sumX.end(), 0) / sumX.size();
					p.y = std::accumulate(sumY.begin(), sumY.end(), 0) / sumY.size();
					p.r = std::accumulate(sumR.begin(), sumR.end(), 0) / sumR.size();
					p.g = std::accumulate(sumG.begin(), sumG.end(), 0) / sumG.size();
					p.b = std::accumulate(sumB.begin(), sumB.end(), 0) / sumB.size();
					*c = p;
				}
			}
		}

		for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it)
		{
			it->minDist = DBL_MAX;
		}

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

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			image.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
		}
	}


	for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it) //Projde všechny body
	{
		for (std::vector<Point>::iterator c = centroids.begin(); c != centroids.end(); ++c) //Pøes všechny centroidy
		{
			int clusterId = c - begin(centroids); //Zjištìní id pro cluster

			if (clusterId == it->cluster) //Tenhle bod má tenhle cluster pak vykreslím na tomto bodì tuto barvu
			{
				image.at<cv::Vec3b>(it->x, it->y) = cv::Vec3b(c->b, c->g, c->r);
			}
		}
	}

	cv::namedWindow("Output", cv::WINDOW_NORMAL);
	imshow("Output", image);
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

vector<vector<float>> SLIC::init(int S, int K, cv::Mat & input)
{

	vector<vector<float>> res;
	float calcS = int(S);
	int counter = 0;
	int colCounter = 1;
	int rowCounter = 0;


	while (counter++ < K) {
		int posX = colCounter * calcS;

		if (posX >= input.cols) {
			rowCounter++;
			colCounter = 1;
			posX = colCounter * calcS;
		}
		else {
			colCounter++;
		}

		int posY = rowCounter * calcS;

		cv::Vec3b  rgb = input.at<cv::Vec3b >(posY, posX);

		//cout << rgb << endl;

		vector<float> centroid{ float(rgb[0]) ,float(rgb[1]) ,float(rgb[2]) , float(posX), float(posY) };


		res.push_back(centroid);
	}



	return res;
}

void SLIC::drawPoints(vector<vector<float>> centroids, cv::Mat & input)
{
	for (int i = 0; i < centroids.size(); i++)
	{
		int y = centroids.at(i).at(3);
		int x = centroids.at(i).at(4);
		cv::circle(input, cv::Point(x, y), 3, CV_RGB(255, 0, 0), CV_FILLED);
	}
}

float SLIC::distance(vector<float> a, vector<float> b, float m, float S)
{

	vector<float> aRGB = { a.at(0), a.at(1), a.at(2) };
	vector<float> bRGB = { a.at(0), a.at(1), a.at(2) };

	vector<float> aXY = { a.at(3), a.at(4) };
	vector<float> bXY = { a.at(3), a.at(4) };


	float dRGB = Calculator::calculate_eucladian_distance(aRGB, bRGB);
	float dXY = Calculator::calculate_eucladian_distance(aXY, bXY);
	float prio = m / S;
	float res = dRGB + (prio / dXY);

	return res;
}
