#!/usr/bin/python3
#-*- coding: utf-8 -*-
참=True
거짓=False

class 유한상태기계_조작법(object):

    def __init__(자신):
        자신.상태 = []
        자신.문자 = []
        자신.전이함수 = {}
        자신.시작상태 = 0
        자신.끝상태 = []

    def 상태_추가(자신,각상태):
        if type(각상태) is str and 각상태 != '':
            자신.상태.append(각상태)
        else:
            raise Exception("상태추가 오류")

    def 상태들_추가(자신,상태들):
        for 각상태 in 상태들:
            자신.상태_추가(각상태)

    def 문자_추가(자신,각문자):
        if type(각문자) is str and 각문자 != '' :
            자신.문자.append(각문자)
        else:
            raise Exception("문자추가 오류")

    def 문자들_추가(자신,문자들):
        for 각문자 in 문자들:
            자신.문자_추가(각문자)

    def 규칙_추가(자신, 전상태, 문자, 후상태):
        if not ((type(전상태) is str) and (type(문자) is str) and (type(후상태) is str)):
            raise Exception("문자가 아님")
        if 전상태 not in 자신.전이함수:
            자신.전이함수[전상태] = dict()
        자신.전이함수[전상태][문자] = 후상태

    def 시작상태_추가(자신,시작문자):
        자신.시작상태 = 시작문자

    def 끝상태_추가(자신,끝문자):
        자신.끝상태.append(끝문자)

    def 끝상태들_추가(자신,끝상태들):
        for 각상태 in 끝상태들:
            자신.끝상태_추가(각상태)

    def 준비(자신):
        for 각상태 in 자신.전이함수:
            if 각상태 not in 자신.상태:
                return 거짓
            for 각문자 in 자신.전이함수[각상태]:
                if 각문자 not in 자신.문자:
                    return 거짓
        if 자신.시작상태 not in 자신.상태:
            return 거짓
        for 각상태 in 자신.끝상태:
            if 각상태 not in 자신.상태:
                return 거짓

        자신.상태.append(0)
        for 각상태 in 자신.상태:
            if 각상태 not in 자신.전이함수:
                자신.전이함수[각상태] = dict()
            for 각문자 in 자신.문자:
                if 각문자 not in 자신.전이함수[각상태]:
                    자신.전이함수[각상태][각문자] = 0

        return 참


class 유한상태기계_객체(object):

    def __init__(자신,조작법):
        자신.상태 = 조작법.상태
        자신.문자 = 조작법.문자
        자신.전이함수 = 조작법.전이함수
        자신.현재상태 = 조작법.시작상태
        자신.끝상태 = 조작법.끝상태

    def 걸음(자신,문자):
        if not 문자 in 자신.문자:
            raise Exception("문자가 리스트 안에 없음")
        자신.현재상태 = 자신.전이함수[자신.현재상태][문자]

    def 총입력(자신,문자들):
        for 각문자 in 문자들:
            자신.걸음(각문자)

    def 정상종료(자신):
        return 자신.현재상태 in 자신.끝상태


def my_error(errorstr):
    print(errorstr)
    exit(-1)

def 한줄얻기(파일객체):
    입력 = 파일객체.readline()
    if len(입력) == 1 and 입력 == '\n':
        return ''
    if 입력[-2:] == '\r\n':
        return 입력[:-2]
    if 입력[-1:] == '\n':
        return 입력[:-1]
    return 입력


def 개행문자제거(입력):
    if len(입력) == 1 and 입력 == '\n':
        return ''
    if 입력[-2:] == '\r\n':
        return 입력[:-2]
    if 입력[-1:] == '\n':
        return 입력[:-1]


def 파일에서_유한상태기계_조작법_얻기(파일이름):

    유한상태파일객체 = open(파일이름, 'r')

    조작법 = 유한상태기계_조작법()
    완료된 = [거짓,거짓,거짓,거짓,거짓]
    걸음들 = ['state','input symbol','state transition function', 'initial state','final state']
    현재줄 = ''
# 상태 받기
    def 상태():
        현재줄 = 한줄얻기(유한상태파일객체)
        조작법.상태들_추가(현재줄.split(','))
        return 현재줄

# 문자 받기
    def 문자():
        현재줄 = 한줄얻기(유한상태파일객체)
        조작법.문자들_추가(현재줄.split(','))
        return 현재줄


# 전이 함수
    def 전이():
        현재줄 = 한줄얻기(유한상태파일객체)

        while (len(현재줄.split(',')) == 3):
            sp = 현재줄.split(',')
            조작법.규칙_추가(sp[0], sp[1], sp[2])
            현재줄 = 한줄얻기(유한상태파일객체)
        return 현재줄

# 초기 상태
    def 시작():
        현재줄 = 한줄얻기(유한상태파일객체)
        조작법.시작상태_추가(현재줄)
        return 현재줄

# 최종 상태
    def 끝():
        try:
            현재줄 = 한줄얻기(유한상태파일객체)
            조작법.끝상태들_추가(현재줄.split(','))
        except:
            my_error("최종상태 처리 오류")
        return 현재줄

    함수들 = [상태, 문자, 전이, 시작, 끝]
    while 완료된 != [참,참,참,참,참]:
        while 현재줄.lower() not in 걸음들:
            현재줄 = 한줄얻기(유한상태파일객체)
        위치 = 걸음들.index(현재줄.lower())
        if 완료된[위치]:
            raise Exception("똑같은 입력이 두번")
        현재줄 = 함수들[위치]()
        완료된[위치] = 참

    if not 조작법.준비():
        raise Exception('준비 오류')

    return 조작법


if __name__ == "__main__":
    조작법 = 파일에서_유한상태기계_조작법_얻기('dfa.txt')
    입력파일 = open('input.txt','r')
    출력파일 = open('output.txt','w')
    for 각입력 in 입력파일:
        한유한상태기계 = 유한상태기계_객체(조작법)
        한줄 = 개행문자제거(각입력)
        한유한상태기계.총입력(한줄)
        if 한유한상태기계.정상종료():
            출력파일.write("네" + '\n')
        else:
            출력파일.write("아니요" + '\n')

