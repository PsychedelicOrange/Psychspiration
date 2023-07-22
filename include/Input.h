class Input{
    EventHandler eventHandler;
    Window* window;
    Input(Window* window)
    {
        this->window = window;
        // set window call back to key_callback
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        controlTable[GLFWKeyToControl[key]](action);
    }
    void processHoldKeys(GLFWindow* window)
    {
        
    }
}

class Player {
    void walkForward();
    void jump();
    Player(Input* input)
    {
        input->registerCallbacK("FORWARD_KEY",walkForward);
        input->registerCallbacK("JUMP_KEY",jump);
    }
}
