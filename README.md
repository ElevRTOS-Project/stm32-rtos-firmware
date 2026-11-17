# 🛗 STM32 FreeRTOS Elevator Firmware

Jetson(상위 제어/UI)과 연동하여 엘리베이터의 실시간 모터 제어, 센서 입력 및 비상 안전 로직을 수행하는 STM32 기반 FreeRTOS 펌웨어 저장소입니다.

---

## 🌿 1. 브랜치 전략 (Branch Strategy)

3인 병렬 개발 시 코드 충돌을 방지하고 안정적인 빌드 상태를 유지하기 위해 기능 단위 브랜치를 분리하여 운영합니다.

* **`main`** : 하드웨어 통합 테스트까지 완료된 안정화 브랜치 (직접 Push 금지)
* **`dev`** : 각 파트의 기능 구현이 끝난 후 병합(Merge)하여 통합 테스트를 진행하는 기준 브랜치
* **`feat/기능명`** : 개인별/기능별 독립 작업 브랜치 (`dev`에서 분기)
  * `feat/drive-motor` : 수직 DC 모터 PWM, 엔코더 카운트, 포토센서 EXTI 제어
  * `feat/door-safety` : 문 개폐 스텝모터 펄스, 추락 방지 서보모터 제어
  * `feat/comm-input` : Jetson 연동 통신 및 층 선택 버튼 디바운싱 처리

---

## ⚠️ 2. STM32CubeIDE 협업 필수 규칙 (충돌 방지)

하나의 MCU에 3명의 코드가 합쳐지므로 아래 규칙을 반드시 준수합니다.

1. **`.ioc` 파일 임의 수정 금지**
   * 핀맵(GPIO), 타이머(TIM), FreeRTOS 태스크/큐 설정 변경이 필요한 경우 팀원 전체 합의 후 **담당자 1명이 대표로 수정하여 `dev`에 반영**합니다.
   * 개인 `feat/*` 브랜치에서 각자 Code Generation을 실행하면 `main.c`와 `freertos.c`에서 대규모 충돌이 발생합니다.
2. **모듈 파일 분리 (`main.c` 직접 코딩 지양)**
   * `main.c`의 `USER CODE` 영역에는 초기화 함수(`*_Init()`)만 호출하고, 실제 제어 로직은 각 파트의 독립된 `.c / .h` 파일에서 작성합니다.
   * 타 파트의 전역 변수를 직접 참조하지 않고, `elevator_common.h`에 정의된 **FreeRTOS Queue 및 Event Flags**를 통해서만 데이터를 주고받습니다.
3. **빌드 확인 후 PR (Pull Request)**
   * 로컬 STM32CubeIDE에서 `0 Errors`로 정상 빌드되는 것을 확인한 뒤에만 PR을 생성합니다.

---

## 💬 3. 커밋 메시지 컨벤션 (Commit Convention)

커밋 메시지는 `태그: 요약` 형식으로 작성합니다.

* **`feat`** : 새로운 하드웨어 드라이버, 태스크, 제어 로직 추가
* **`fix`** : 버그 수정, 타이밍/인터럽트 오류 및 채터링(Chattering) 해결
* **`refactor`** : 동작 변경 없는 코드 구조 개선 및 함수 분리
* **`chore`** : `.ioc` 설정 변경, 핀맵 수정, 빌드 설정 업데이트
* **`docs`** : `README.md`, 주석, 통신 프로토콜 문서 수정

> **예시:**
> * `feat: implement TIM2 encoder mode and DC motor PWM control`
> * `fix: resolve button chattering issue with 20ms polling`
> * `chore: update .ioc pinmap for photo sensors (EXTI0~3)`

---

## 🔄 4. 기본 작업 흐름 (Git Workflow)

```bash
# 1. 작업 시작 전 최신 dev 브랜치 동기화
git checkout dev
git pull origin dev

# 2. 내 작업 브랜치 생성 및 이동 (최초 1회)
git checkout -b feat/내기능이름

# 3. 코드 작성 및 커밋
git add .
git commit -m "feat: 구현 내용 요약"

# 4. 원격 저장소에 내 브랜치 푸시 후 GitHub에서 dev로 Pull Request 생성
git push -u origin feat/내기능이름