#ifndef LEFT_UTILS_H
#define LEFT_UTILS_H


class Left_utils
{
public:
    Left_utils();
    //切换到哪个窗口
    enum left_button
    {
         hello=0 ,whole_simulation,development_board , component,deploy_management,log
    };
    //切换的方式
    enum Switch_mode
    {
         only_switch=0 ,switch_update,only_update
    };
};

#endif // LEFT_UTILS_H
