# WordReminder
윈도우용 단어 암기 프로그램

[나눔고딕](https://hangeul.naver.com/2017/nanum) 폰트 설치를 추천합니다.

## 사용 방법
### 메인 화면
<p float="left">
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/MainWindow.png" width="49%" />
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/MainWindow_MultiplayClicked.png" width="49%" />
</p>

프로그램을 실행하면 가장 먼저 볼 수 있는 화면입니다. 단어 암기하기 버튼을 눌러 단어를 암기할 수 있으며, 단어장 만들기/수정하기 버튼을 눌러 암기에 필요한 단어장을 만들거나 수정할 수도 있습니다. 멀티 플레이 버튼을 눌러 친구와 함께 단어를 암기할 수도 있습니다. 멀티 플레이 버튼을 누르면 버튼이 5초간 서버 만들기와 서버 접속하기 버튼으로 나뉘는데, 멀티 플레이 서버를 직접 열거나 친구가 만든 멀티 플레이 서버에 접속할 수 있습니다.

### 단어장 만들기/수정하기
![VocabularyWindow](https://github.com/kmc7468/WordReminder/blob/master/img/VocabularyWindow.png)

메인 화면에서 단어장 만들기/수정하기 버튼을 누르면 볼 수 있는 화면입니다. 단어장을 새로 만들거나, 기존 단어장을 불러와 편집할 수 있습니다. 단어장에는 적어도 뜻이 다른 5개 이상의 단어가 등록되어야 저장할 수 있습니다. 동음이의어나 다의어는 공식적으로 지원하지 않으므로, 이러한 경우에는 뜻을 입력하는 칸에 해당 단어가 가진 뜻을 적절히 컴마 등으로 구분해 입력해야 합니다. 발음은 꼭 입력하지 않아도 됩니다.

### 싱글 플레이
![QuestionOptionWindow](https://github.com/kmc7468/WordReminder/blob/master/img/QuestionOptionWindow.png)

메인 화면에서 단어 암기하기 버튼을 누르면 볼 수 있는 화면입니다. 암기할 단어장을 선택하고, 문제 유형을 선택할 수 있습니다.

![QuestionWindow](https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow.png)

시작하기 버튼을 누르면 단어를 암기할 수 있습니다. 선택된 단어장에서 선택한 문제 유형에 따라 5지선다형 문제가 자동으로 생성됩니다.

![StatisticWindow](https://github.com/kmc7468/WordReminder/blob/master/img/StatisticWindow.png)

그만 외우기 버튼을 누르면 암기를 종료할 수 있습니다. 이 화면에서는 잘 외우지 못한 단어 목록을 확인하고, 잘 외우지 못한 단어만 모아서 다시 암기할 수도 있습니다. 이러한 경우 적어도 뜻이 다른 5개 이상의 단어를 틀렸어야 합니다.

### 멀티 플레이
<p float="left">
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/MultiplayStartWindow_ServerCreation.png" width="49%" />
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/MultiplayStartWindow_ServerJoining.png" width="49%" />
</p>

메인 화면에서 서버 만들기, 서버 접속하기 버튼을 누르면 볼 수 있는 화면입니다. 서버 만들기를 눌렀을 경우 컴퓨터의 외부 IP를 확인하고, 서버를 열 포트와 플레이 모드, 자신의 역할을 고를 수 있습니다. 서버 접속하기 버튼을 눌렀을 경우 접속할 서버의 주소를 입력할 수 있습니다.

턴제 모드는 두 사람의 역할이 매 턴마다 바뀌는 플레이 모드이고, 역할 고정 모드는 역할이 바뀌지 않는 플레이 모드입니다.

#### 턴제 모드
<p float="left">
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow_Examiner1.png" width="49%" />
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow_Examiner2.png" width="49%" />
</p>

턴제 모드에서, 먼저 출제자는 컴퓨터가 생성한 5개의 선지를 확인할 수 있습니다. 출제자는 이 중 하나의 단어를 골라 응시자에게 해당 단어를 맞히도록 할 수 있습니다. 응시자가 문제를 맞힐 때까지 출제자는 대기하게 됩니다.

<p float="left">
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow_Examinee1.png" width="49%" />
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow_Examinee2.png" width="49%" />
</p>

응시자는 먼저 출제자가 단어를 고르는 것을 대기하게 됩니다. 출제자가 단어를 고르면, 응시자는 출제자의 컴퓨터가 생성한 5개의 선지를 확인할 수 있습니다. 응시자는 제시된 문제를 읽고, 적절한 답을 골라야 합니다. 이때, 답은 출제자가 정한 단어가 됩니다.

문제의 유형은 서버 생성자의 설정에 따릅니다. 만약 1가지 유형만 골랐다면 해당 유형의 문제만 출제되고, 2가지 유형을 골랐다면 랜덤으로 유형이 결정됩니다.

응시자가 올바른 답을 고르면 1회의 턴이 끝나고, 출제자와 응시자의 역할이 바뀝니다. 게임은 언제든지 그만 외우기 버튼을 눌러 중지할 수 있습니다. 그만 외우기 버튼을 누르면 싱글 플레이와 같이 잘 외우지 못한 단어 목록을 확인하고, 잘 외우지 못한 단어만 모아서 다시 암기할 수도 있습니다. 단, 다시 암기하는 것은 멀티 플레이가 아닌 싱글 플레이로 진행됩니다.

### 역할 고정 모드
<p float="left">
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/ExaminerWindow_1.png" width="49%" />
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/ExaminerWindow_2.png" width="49%" />
</p>

역할 고정 모드에서, 먼저 출제자는 단어 목록을 확인할 수 있습니다. 출제자는 이 중 하나의 단어를 골라 응시자에게 해당 단어를 맞히도록 할 수 있습니다. 선택한 단어 외의 선지는 컴퓨터가 자동으로 생성합니다. 응시자가 문제를 맞힐 때까지 출제자는 대기하게 됩니다.

<p float="left">
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow_FixedExaminee1.png" width="49%" />
	<img src="https://github.com/kmc7468/WordReminder/blob/master/img/QuestionWindow_FixedExaminee2.png" width="49%" />
</p>

응시자는 먼저 출제자가 단어를 고르는 것을 대기하게 됩니다. 출제자가 단어를 고르면, 응시자는 출제자가 고른 선지와 컴퓨터가 생성한 4개의 선지를 확인할 수 있습니다. 응시자는 제시된 문제를 읽고, 적절한 답을 골라야 합니다. 이때, 답은 출제자가 정한 단어가 됩니다.

문제의 유형은 서버 생성자의 설정에 따릅니다. 만약 1가지 유형만 골랐다면 해당 유형의 문제만 출제되고, 2가지 유형을 골랐다면 랜덤으로 유형이 결정됩니다.

응시자가 올바른 답을 고르면, 출제자는 다시 응답자에게 맞히도록 할 단어를 고를 수 있게되며, 응답자는 출제자가 단어를 고르는 것을 다시 대기하게 됩니다.

만약 출제자나 응답자 둘 중 한 쪽이 역할 변경 요청하기 버튼을 누르면, 상대방에게 출제자와 응답자의 역할을 바꿀 것을 요청하게 됩니다. 만약 상대방이 역할 변경을 수락할 경우 출제자는 응답자가, 응답자는 출제자가 되어 위에서 설명한 과정을 진행하게 됩니다.