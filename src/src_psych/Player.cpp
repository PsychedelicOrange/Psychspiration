#include <Player.h>
#include <glm/gtx/euler_angles.hpp>

 Player::Player(RObject* obj, Input& input, RCameraObject* camera) : obj(obj), input(input), body(obj->rigidBody), camera(camera), forward(glm::vec3(0, 0, -1)), up(glm::vec3(0, 1, 0))
{
	//body->activate(true);
	setUpEvents(input);
}

 void Player::updateCamera(glm::vec3 up, float minDistance)
{
	glm::vec3 position(obj->transform[3]);
	btVector3 velocity = { obj->rigidBody->getLinearVelocity().x(),obj->rigidBody->getLinearVelocity().y(),obj->rigidBody->getLinearVelocity().z() };
	auto speed = std::sqrt(velocity.length());
	float distance = std::max(speed, minDistance);
	auto fov = speed * 4 + 45;
	fov = fov > 180 ? 180 : fov;
	camera->camera->yfov = fov;
	//camera->Up = up;
	camera->Position = position - (forward * distance);
	camera->Front = position - camera->Position;
	camera->Position = position - (forward * distance);
	camera->Position += glm::vec3(0,1,0);
}

 void Player::applyForce(int dir, float deltatime)
{
	int magnitude = 1;
	btVector3 front = { forward.x , forward.y, forward.z };
	btVector3 Up = { camera->Up.x,camera->Up.y,camera->Up.z };
	btVector3 force;
	if (dir == 0)
		force = front;
	else if (dir == 1)
		force = -front;
	else if (dir == 2)
		force = -btCross(front, Up);
	else if (dir == 3)
		force = btCross(front, Up);
	force = force.normalized() * magnitude;
	body->applyCentralForce(force);
}
 void Player::applyTurnTorque(int dir, float deltatime)
 {
	 btVector3 torque{ btVector3(-0.0001,0,0) };
	 if (dir == 0)
		 body->applyTorqueImpulse(torque);
	 else if (dir == 1)
		 body->applyTorqueImpulse(-torque);
	 torque = btVector3(0,0,0.0001);
	 if (dir == 2)
		 body->applyTorqueImpulse(torque);
	 else if (dir == 3)
		 body->applyTorqueImpulse(-torque);
 }
 void Player::setUpEvents(Input& input)
{
	input.registerKeyCallback("FORWARD_KEY", 3, [this, &input]() {
		applyTurnTorque(0, input.deltaTime);
		});
	input.registerKeyCallback("BACKWARD_KEY", 3, [this, &input]() {
		applyTurnTorque(1, input.deltaTime);
		});
	input.registerKeyCallback("LEFT_KEY", 3, [this, &input]() {
		applyTurnTorque(2, input.deltaTime);
		//this->body->applyCentralForce(btVector3(-10, 0, 0));
		});
	input.registerKeyCallback("RIGHT_KEY", 3, [this, &input]() {
		applyTurnTorque(3, input.deltaTime);
		//this->body->applyCentralForce(btVector3(10, 0, 0));
		});
	input.registerKeyCallback("RUN_KEY", 3, [this, &input]() {
		this->body->applyCentralForce(btVector3(0, 0, -10));
		});
	input.registerPerTickCallback([this]() {tickUpdate(); });
}

//frisbee parameters
const float cdo = 0.08, cda = 2.72, clo = 0.1, cla = 1.4, alphaKnot = glm::radians(-4.0f);
const float air_density = 1.23, area = 0.0568;
void Player::tickUpdate() /// runs every frame
{
	////updating up vector
	btTransform q = body->getWorldTransform();
	glm::mat4 rotationMatrix(1.0f);
	
	q.getOpenGLMatrix(&rotationMatrix[0][0]);

	up = glm::vec4(0,1,0,1.0) * rotationMatrix;
	up.x *= -1;
	updateCamera(up,5); // updates camera to follow frisbee

	float x_angle = -atan(up.x / up.y);
	float z_angle = -atan(up.z / up.y);
	angle_of_attack = x_angle;	

	/*btVector3 force;
	force *= 5;
	body->applyCentralForce(force);*/
	btVector3 btUp(up.x, up.y, up.z);
	btUp.normalize();
	velocity = body->getLinearVelocity();
	btScalar speed = velocity.length();
	//// glm::extractEulerAngleYZY()
	// apply drag and lift
	////btVector3 frontcam(camera->Front.x,camera->Front.y,camera->Front.z);
	//velocity.setY(0);
	//if (speed == 0)angle_of_attack = 0;
	btVector3 drag{}, lift{};
	float drag_coeficient = cdo + (cda * pow(z_angle - alphaKnot, 2)); //std::cout << "\ndrag_coeficient: " << drag_coeficient;
	drag = 0.5f * drag_coeficient * area  * speed * -velocity;
	float lift_coeficient = clo + cla * z_angle; //std::cout << "\nlift_coeficient: " << lift_coeficient;
	lift = 0.5f * lift_coeficient * area * speed * speed * btUp;
	//std::cout <<"\nLift force :" << lift.x() << " " << lift.y() << " " << lift.z() << ";";
	//std::cout <<"\nDrag force :" << drag.x() << " " << drag.y() << " " << drag.z() << ";";
	//lift = lift+drag;
	body->applyCentralForce(lift);
	body->applyCentralForce(drag);
}
