# SummerProject-WinCPS-Tools

## OTP认证Android端
输入主机端nonce，获得PIN码。

PIN码计算：SHA-1(userSecret+SHA-1(userSecret+currentTime/30+nonce))

![taEcC.gif](https://storage7.cuntuku.com/2018/07/09/taEcC.gif)
