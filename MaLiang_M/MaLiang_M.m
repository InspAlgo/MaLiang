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
function enhance_restore_Callback(hObject, eventdata, handles)  % ͼ��ԭ00; 
% hObject    handle to enhance_restore (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
% ����ʹ�� ά���˲� ����ͼ���˶�ģ����ԭ
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
img = double(img);  %������ת��Ϊdouble�ͺ���ж�ά����Ҷ�任��ͼ������кܶദ����������
f_img = fft2(img);  % fft2() �ǿ��ٸ���Ҷ�任����
f_img = fftshift(f_img);  % fftshift() ������Ҷ�任����Ƶ�ʳɷ��Ƶ�Ƶ������,Ŀ����Ϊ�˽�ͼ���ݺỮ�ֳ��Ŀ飬�Խ��߻���
% �����任�Ժ��ͼ�� f_img �����ǵ�Ƶ������Ƶ������

% ���²�����Ϊ�˻�ÿ�����ʾ��ͼ��
% ���ֱ����ʾ f_img���õ��ĺܿ�����һ��ȫ�׵�ͼ����Ϊ f_img �����ص�ֵһ�㶼�ܴ�Զ���� 255
% �� matlab �� imshow() �����ڰ׶�Ӧ 0-255�����Բ�����һ�������ͼ����ʾ���ǰ�ɫ��

B = abs(real(f_img)); % f_img �п����Ǹ�ֵ���������������ĶԳƵģ�����Ϊ������
[m, n] = size(B);  % �������������
R = reshape(B, 1, m*n);  % ��Bӳ���1��m*n�еľ���
temp = mapminmax(R, 0, 255);  % �� R ������ֵ��һ����0-255֮��
result = reshape(temp,m,n);
axes(handles.axes_result);
imshow(result);

% --------------------------------------------------------------------
function trans_inv_Callback(hObject, eventdata, handles)
% hObject    handle to trans_inv (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
% ����Ҷ�任���ͼ���м䲿��Ϊ��Ƶ���֣�Խ�����Ƶ��Խ��
function trans_fourier_Callback(hObject, eventdata, handles)  % ����Ҷ�任
% hObject    handle to trans_fourier (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global working_image;
global result_image;

if isempty(working_image)
    return;
end

img = rgb2gray(working_image);
img = double(img);  %������ת��Ϊdouble�ͺ���ж�ά����Ҷ�任��ͼ������кܶദ����������
f_img = fft2(img);  % fft2() �ǿ��ٸ���Ҷ�任����
f_img = fftshift(f_img);  % fftshift() ������Ҷ�任����Ƶ�ʳɷ��Ƶ�Ƶ������,Ŀ����Ϊ�˽�ͼ���ݺỮ�ֳ��Ŀ飬�Խ��߻���
% �����任�Ժ��ͼ�� f_img �����ǵ�Ƶ������Ƶ������

B = abs(real(f_img)); % f_img �п����Ǹ�ֵ���������������ĶԳƵģ�����Ϊ������
[m, n] = size(B);  % �������������
R = reshape(B, 1, m*n);  % ��Bӳ���1��m*n�еľ���
temp = mapminmax(R, 0, 255);  % �� R ������ֵ��һ����0-255֮��
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
% �κ�ʵ�Գƺ����ĸ���Ҷ�任��ֻ����������ұ任�Ǹ���Ҷ�任�����������ұ任�Ǽ�DFT����Ҫ����
function trans_discrete_cosine_Callback(hObject, eventdata, handles)  % ��ɢ���ұ任
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
dct_gray_img = dct2(gray_img);  % ��ͼ�� DCT �任

axes(handles.axes_img);
imshow(log(abs(dct_gray_img))),colormap(gray(4)),colorbar;

dct_gray_img(abs(dct_gray_img)<0.1)=0;  % �Ծ����������
I = idct2(dct_gray_img)/255;  % DCT ��任
result_image = I;
axes(handles.axes_result);
imshow(I);

% --------------------------------------------------------------------
% һ�ִ��ڴ�С�̶�������״�ɸı䣬���������ʱƵ�ֲ���������Ҫ��ı任
function trans_wavelet_Callback(hObject, eventdata, handles)  % С���任
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
[cA,cH,cV,cD] = dwt2(img,'haar');  % ���ж�άС���任
X = size(working_image, 1);
result1 = [wcodemat(cA), log(abs(cH));log(abs(cV)), log(abs(cD))];  % ��Ƶ���� ˮƽϸ�ڷ��� ��ֱϸ�ڷ��� �Խ���ϸ�ڷ���
axes(handles.axes_img);
imshow(result1,colormap(gray(4))),colorbar;

result2 = idwt2(cA,cH,cV,cD,'haar');
result2 = imadjust(result2,[0 1],[1 0]);  % ��Ƭ����ʱΪ����������ͼ
result2 = imlincomb(result2,img);  % ��ԭͼ��������ͼ����
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

[filename, pathname] = uigetfile({'*.bmp';'*.jpg';'*.png'},'ѡ��ͼƬ');
if isequal(filename, 0) || isequal(pathname, 0)
    errordlg('δѡȡͼƬ','��ܰ��ʾ');
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

[filename, pathname, filterindex] = uiputfile({'*.bmp';'*.jpg';'*.png'},'����ͼƬ');

if isequal(filename, 0) || isequal(pathname, 0)
    errordlg('δ����ͼƬ','��ܰ��ʾ');
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
