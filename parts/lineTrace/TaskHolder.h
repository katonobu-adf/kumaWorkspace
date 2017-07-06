#ifndef TASKHOLDER_H
#define TASKHOLDER_H

class TaskHolder {
public:
    TaskHolder();
    int run();

protected:
    void tail_control(signed int angle);
};


#endif  /* TASKHOLDER_H */
