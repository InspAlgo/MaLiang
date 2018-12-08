function varargout = MaLiang_M(varargin)
% MALIANG_M MATLAB code for MaLiang_M.fig
%      MALIANG_M, by itself, creates a new MALIANG_M or raises the existing
%      singleton*.
%
%      H = MALIANG_M returns the handle to a new MALIANG_M or the handle to
%      the existing singleton*.
%
%      MALIANG_M('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in MALIANG_M.M with the given input arguments.
%
%      MALIANG_M('Property','Value',...) creates a new MALIANG_M or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before MaLiang_M_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to MaLiang_M_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help MaLiang_M

% Last Modified by GUIDE v2.5 16-Nov-2018 20:09:39

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @MaLiang_M_OpeningFcn, ...
                   'gui_OutputFcn',  @MaLiang_M_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before MaLiang_M is made visible.
function MaLiang_M_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to MaLiang_M (see VARARGIN)

% Choose default command line output for MaLiang_M
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes MaLiang_M wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = MaLiang_M_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1


% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --------------------------------------------------------------------
function menu_file_Callback(hObject, eventdata, handles)
% hObject    handle to menu_file (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function menu_trans_Callback(hObject, eventdata, handles)
% hObject    handle to menu_trans (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function menu_enhance_Callback(hObject, eventdata, handles)
% hObject    handle to menu_enhance (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function enhance_restore_Callback(hObject, eventdata, handles)  % 图像复原00; 
% hObject    handle to enhance_restore (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
% 首先使用 维纳滤波 进行图像运动模糊复原
PSF = fspecial('motion', 50, 75);
MF = imfilter(working_image, PSF, 'circular');
noise = imnoise(zeros(size(working_image)), 'gaussian', 0, 0.001);
MFN = imadd(MF, im2uint8(noise));
NSR = sum(noise(:).^2) / sum(MFN(:).^2);
working_image = deconvwnr(MFN, PSF, NSR);
axex(handles.axes_result);
imshow(working_image);


% --------------------------------------------------------------------
function trans_spectrum_Callback(hObject, eventdata, handles)
% hObject    handle to trans_spectrum (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;

if isempty(working_image)
    return;
end

img = rgb2gray(working_image);
img = double(img);  %将矩阵转化为double型后进行二维傅里叶变换，图像计算中很多处理不能用整型
f_img = fft2(img);  % fft2() 是快速傅里叶变换函数
f_img = fftshift(f_img);  % fftshift() 将傅里叶变换的零频率成分移到频谱中心,目的是为了将图像纵横划分成四块，对角线互换
% 经过变换以后的图像 f_img 中心是低频，往外频率增大

% 以下操作是为了获得可以显示的图像
% 如果直接显示 f_img，得到的很可能是一个全白的图，因为 f_img 内像素的值一般都很大，远大于 255
% 而 matlab 的 imshow() 函数黑白对应 0-255，所以不做归一化处理的图像显示都是白色的

B = abs(real(f_img)); % f_img 有可能是负值，而且数据是中心对称的，中心为最亮点
[m, n] = size(B);  % 获得行数、列数
R = reshape(B, 1, m*n);  % 将B映射成1行m*n列的矩阵
temp = mapminmax(R, 0, 255);  % 将 R 的像素值归一化到0-255之间
result = reshape(temp,m,n);
axes(handles.axes_result);
imshow(result);

% --------------------------------------------------------------------
function trans_inv_Callback(hObject, eventdata, handles)
% hObject    handle to trans_inv (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
% 傅里叶变换后的图像，中间部分为低频部分，越靠外边频率越高
function trans_fourier_Callback(hObject, eventdata, handles)  % 傅里叶变换
% hObject    handle to trans_fourier (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
global result_image;

if isempty(working_image)
    return;
end

img = rgb2gray(working_image);
img = double(img);  %将矩阵转化为double型后进行二维傅里叶变换，图像计算中很多处理不能用整型
f_img = fft2(img);  % fft2() 是快速傅里叶变换函数
f_img = fftshift(f_img);  % fftshift() 将傅里叶变换的零频率成分移到频谱中心,目的是为了将图像纵横划分成四块，对角线互换
% 经过变换以后的图像 f_img 中心是低频，往外频率增大

B = abs(real(f_img)); % f_img 有可能是负值，而且数据是中心对称的，中心为最亮点
[m, n] = size(B);  % 获得行数、列数
R = reshape(B, 1, m*n);  % 将B映射成1行m*n列的矩阵
temp = mapminmax(R, 0, 255);  % 将 R 的像素值归一化到0-255之间
result = reshape(temp,m,n);
axes(handles.axes_img);
imshow(result),colormap(gray(4)),colorbar;

f_img(abs(f_img)<5000)=0;
f_img = ifftshift(f_img);
result2 = ifft2(f_img);
result_image = result2;
axes(handles.axes_result);
imshow(result2,[0,255]);



% --------------------------------------------------------------------
% 任何实对称函数的傅里叶变换中只含余弦项，余弦变换是傅里叶变换的特例，余弦变换是简化DFT的重要方法
function trans_discrete_cosine_Callback(hObject, eventdata, handles)  % 离散余弦变换
% hObject    handle to trans_discrete_cosine (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
global result_image;

if isempty(working_image)
    return;
end

gray_img = rgb2gray(working_image);
gray_img = double(gray_img);
dct_gray_img = dct2(gray_img);  % 对图像 DCT 变换

axes(handles.axes_img);
imshow(log(abs(dct_gray_img))),colormap(gray(4)),colorbar;

dct_gray_img(abs(dct_gray_img)<0.1)=0;  % 对矩阵进行量化
I = idct2(dct_gray_img)/255;  % DCT 逆变换
result_image = I;
axes(handles.axes_result);
imshow(I);

% --------------------------------------------------------------------
% 一种窗口大小固定，但形状可改变，因而能满足时频局部化分析的要求的变换
function trans_wavelet_Callback(hObject, eventdata, handles)  % 小波变换
% hObject    handle to trans_wavelet (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
global result_image;

if isempty(working_image)
    return;
end

img = rgb2gray(working_image);
img = double(img);
[cA,cH,cV,cD] = dwt2(img,'haar');  % 进行二维小波变换
X = size(working_image, 1);
result1 = [wcodemat(cA), log(abs(cH));log(abs(cV)), log(abs(cD))];  % 低频分量 水平细节分量 垂直细节分量 对角线细节分量
axes(handles.axes_img);
imshow(result1,colormap(gray(4))),colorbar;

result2 = idwt2(cA,cH,cV,cD,'haar');
result2 = imadjust(result2,[0 1],[1 0]);  % 求负片，此时为部分轮廓线图
result2 = imlincomb(result2,img);  % 将原图和轮廓线图叠加
result_image = result2;
axes(handles.axes_result);
imshow(result2);


% --------------------------------------------------------------------
function file_open_Callback(hObject, eventdata, handles)
% hObject    handle to file_open (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
global result_image;

[filename, pathname] = uigetfile({'*.bmp';'*.jpg';'*.png'},'选择图片');
if isequal(filename, 0) || isequal(pathname, 0)
    errordlg('未选取图片','温馨提示');
    return;
else 
    image_path = [pathname filename];
    image = imread(image_path);
    working_image = image;
    result_image = image;
    axes(handles.axes_img);
    imshow(image);
    axes(handles.axes_result);
    imshow(image);
end

% --------------------------------------------------------------------
function file_save_as_Callback(hObject, eventdata, handles)
% hObject    handle to file_save_as (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global result_image;

if isempty(result_image)
    return;
end

[filename, pathname, filterindex] = uiputfile({'*.bmp';'*.jpg';'*.png'},'保存图片');

if isequal(filename, 0) || isequal(pathname, 0)
    errordlg('未保存图片','温馨提示');
    return;
else 
    save_path = [pathname filename];
    imwrite(result_image, save_path, 'bmp');
end

% --------------------------------------------------------------------
function file_close_Callback(hObject, eventdata, handles)
% hObject    handle to file_close (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
