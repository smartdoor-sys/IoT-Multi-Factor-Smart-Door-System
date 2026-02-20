#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <ctime>
#include <filesystem>
#include <sstream>  

class Database {
public:
    Database() {
        int rc = sqlite3_open("face_recognition.db", &db);
        if (rc) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
            exit(1);
        }

        // Create table if it doesn't exist
        const char* create_table_sql =
            "CREATE TABLE IF NOT EXISTS face_metadata ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT, "
            "image_path TEXT, "
            "status TEXT, "
            "timestamp TEXT);";

        rc = sqlite3_exec(db, create_table_sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << err_msg << "\n";
            sqlite3_free(err_msg);
            exit(1);
        }
    }

    ~Database() {
        sqlite3_close(db);
    }

    void storeFaceMetadata(const std::string& name, const std::string& imagePath, bool status) {
        std::time_t current_time = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

        std::stringstream sql;
        sql << "INSERT INTO face_metadata (name, image_path, status, timestamp) VALUES ('"
            << name << "', '" << imagePath << "', '" << (status ? "success" : "failure") << "', '" << timestamp << "');";

        int rc = sqlite3_exec(db, sql.str().c_str(), 0, 0, &err_msg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << err_msg << "\n";
            sqlite3_free(err_msg);
        }
    }

    std::vector<std::string> getAllUserNames() {
        std::vector<std::string> userNames;
        const char* query = "SELECT name FROM face_metadata";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << sqlite3_errmsg(db) << "\n";
            return userNames;
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            userNames.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }

        sqlite3_finalize(stmt);
        return userNames;
    }

private:
    sqlite3* db;
    char* err_msg;
};

// FaceRecognition class for capturing and recognizing faces

class FaceRecognitionSystem {
public:
    FaceRecognitionSystem() : faceRecognizer(cv::face::LBPHFaceRecognizer::create()) {
        // Load face detection classifier
        if (!face_cascade.load("haarcascade_frontalface_default.xml")) {
            std::cerr << "Error loading face cascade\n";
            exit(1);
        }

        // Initialize camera
        cap.open(0);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open camera.\n";
            exit(1);
        }

        // Ensure that the "images" folder exists
        if (!std::filesystem::exists("images")) {
            std::filesystem::create_directory("images");
        }

        // Train the face recognizer with images already in the database
        trainFaceRecognizer();
    }

    ~FaceRecognitionSystem() {
        cap.release();
    }

    // Start face recognition process
    void startRecognition() {
        std::cout << "Starting face recognition...\n";

        while (true) {
            cv::Mat frame;
            cap >> frame;
            if (frame.empty()) {
                continue;
            }

            // Convert frame to grayscale for detection
            cv::Mat gray;
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            std::vector<cv::Rect> faces;
            face_cascade.detectMultiScale(gray, faces);

            for (const auto& face : faces) {
                cv::Mat faceROI = gray(face);

                // Recognize the face using the trained recognizer
                int predictedLabel = -1;
                double confidence = 0.0;
                faceRecognizer->predict(faceROI, predictedLabel, confidence);

                // If the face is not recognized, add the new face
                if (predictedLabel == -1 || confidence > 100) {
                    std::cout << "Unrecognized face, adding to the database...\n";
                    addNewFace(frame, face);
                } else {
                    std::cout << "Recognized face with ID: " << predictedLabel << "\n";
                }

                // Draw a rectangle around the face
                cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);
            }

            // Display the frame with detected faces
            cv::imshow("Face Recognition", frame);

            if (cv::waitKey(1) == 27) {
                break;  // Escape key pressed, exit the loop
            }
        }
    }

private:
    cv::CascadeClassifier face_cascade;
    cv::VideoCapture cap;
    cv::Ptr<cv::face::LBPHFaceRecognizer> faceRecognizer;
    Database database;

    // Function to train the face recognizer with the images in the database
    void trainFaceRecognizer() {
        std::vector<cv::Mat> images;
        std::vector<int> labels;
        std::vector<std::string> userNames = database.getAllUserNames();

        int label = 0;  // Unique label for each user

        for (const std::string& userName : userNames) {
            std::string imagePath = "images/" + userName + "_face.jpg";
            cv::Mat img = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
            if (!img.empty()) {
                images.push_back(img);
                labels.push_back(label++);
            }
        }

        if (!images.empty()) {
            faceRecognizer->train(images, labels);
            faceRecognizer->save("face_recognizer_model.xml");
            std::cout << "Face recognizer trained with new data.\n";
        } else {
            std::cout << "No images found for training.\n";
        }
    }

    // Function to add a new face to the system and the database
    void addNewFace(const cv::Mat& frame, const cv::Rect& face) {
        // Get the name for the new user
        std::string userName;
        std::cout << "Enter name of the new user: ";
        std::cin >> userName;

        // Crop and save the new face image
        cv::Mat faceROI = frame(face);
        std::string fileName = "images/" + userName + "_face.jpg";
        cv::imwrite(fileName, faceROI);

        // Store the metadata in the database
        database.storeFaceMetadata(userName, fileName, true);

        // Retrain the recognizer with the new face added
        trainFaceRecognizer();
    }
};

int main() {
    FaceRecognitionSystem faceRecognitionSystem;
    faceRecognitionSystem.startRecognition();

    return 0;
}
