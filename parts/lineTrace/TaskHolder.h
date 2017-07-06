#ifndef TASKHOLDER_H
#define TASKHOLDER_H

class TaskHolder {
public:
    TaskHolder();
    virtual int init();
    virtual int run();

protected:
    void tail_control(signed int angle);
    int  subState;
};


#endif  /* TASKHOLDER_H */
