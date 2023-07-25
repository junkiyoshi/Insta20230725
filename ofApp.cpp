#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;

	this->mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > frame_span * 8) { this->log.pop_back(); }

	this->mesh.clear();
	for (int i = 0; i < this->log.size() - 1; i++) {

		this->mesh.addVertex(this->log[i]);
		this->mesh.addColor(ofColor(this->color, ofMap(i, 0, this->log.size(), 255, 0)));
		if (i > 0) {

			this->mesh.addIndex(i);
			this->mesh.addIndex(i - 1);
		}
	}
}

//--------------------------------------------------------------
void Actor::draw() {

	//this->mesh.draw();

	ofSetColor(this->color);
	ofDrawSphere(this->location, 3);
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void Actor::setColor(ofColor color) {

	this->color = color;
}

//--------------------------------------------------------------
ofColor Actor::getColor() {

	return this->color;
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetLineWidth(2);
	ofNoFill();
	ofEnableDepthTest();

	int span = 50;
	for (int x = -100; x <= 100; x += span) {

		for (int y = -100; y <= 100; y += span) {

			for (int z = -100; z <= 100; z += span) {

				this->location_list.push_back(glm::vec3(x, y, z));
			}
		}
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= span) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	ofColor color;
	for (int i = 0; i < 115; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
		this->actor_list.back()->setColor(ofColor(39));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 15;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofTranslate(-5, -5, 0);
	this->cam.setPosition(glm::vec3(0, 0, 400));
	ofRotateY(ofGetFrameNum());

	ofFill();
	for (auto& actor : this->actor_list) {

		actor->draw();
	}

	for (int i = 0; i < this->actor_list.size(); i++) {

		for (int k = i + 1; k < this->actor_list.size(); k++) {

			auto distance = glm::distance(this->actor_list[i]->getLocation(), this->actor_list[k]->getLocation());
			if (distance <= 60) {

				ofSetColor(39, distance > 50 ? ofMap(distance, 50, 60, 200, 0) : 200);
				ofDrawLine(this->actor_list[i]->getLocation(), this->actor_list[k]->getLocation());
			}
		}
	}


	this->cam.end();
}

//--------------------------------------------------------------
glm::vec3 ofApp::make_point(float R, float r, float u, float v) {

	// 数学デッサン教室 描いて楽しむ数学たち　P.31

	u *= DEG_TO_RAD;
	v *= DEG_TO_RAD;

	auto x = (R + r * cos(u)) * cos(v);
	auto y = (R + r * cos(u)) * sin(v);
	auto z = r * sin(u);

	return glm::vec3(x, y, z);
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}