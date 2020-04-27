/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "sharedutils/util_string.h"
#include <cctype>
#include <algorithm>
#include <stdexcept>

#pragma optimize("",off)
const auto SIM_INF = std::numeric_limits<double>::lowest();
const auto SIM_CONSECUTIVE_FACTOR = 10u;

// Source: https://stackoverflow.com/a/41465315/2482983
static double levenshtein(std::vector<std::vector<std::vector<double>>> &memo,const std::string_view &inputWord,const std::string_view &checkWord,const size_t i,const size_t j,const size_t count){
    if(i == inputWord.length() && j == checkWord.length())
		return 0;    
    if(i == inputWord.length())
		return checkWord.length() -j;
    if(j == checkWord.length())
		return inputWord.length() -i;

	if(i >= memo.size())
		memo.resize(i +10);
	if(j >= memo.at(i).size())
		memo.at(i).resize(j +10);
	if(count >= memo.at(i).at(j).size())
		memo.at(i).at(j).resize(count +10,SIM_INF);

    if(memo[i][j][count] != SIM_INF)
		return memo[i][j][count];

	auto ans1 = 0.0;
	auto ans2 = 0.0;
	auto ans3 = 0.0;
	auto ans = 0.0;
	auto c0 = inputWord.at(i);
	auto c1 = checkWord.at(j);
	auto factor = SIM_CONSECUTIVE_FACTOR;
	if(c0 != c1)
	{
		c0 = std::tolower(c0);
		c1 = std::tolower(c1);
		if(c0 == c1)
			factor /= 2; // If case doesn't match, lower the factor
	}
    if(c0 == c1)
	{
        ans1 = levenshtein(memo,inputWord,checkWord,i +1,j +1,count +1) -(factor *(count +1));
        ans2 = levenshtein(memo,inputWord,checkWord,i +1,j,0) +1;
        ans3 = levenshtein(memo,inputWord,checkWord,i,j+1,0) +1;
        ans = std::min(ans1,std::min(ans2,ans3));
    }
	else
	{
        ans1 = levenshtein(memo,inputWord,checkWord,i +1,j,0) +1;
        ans2 = levenshtein(memo,inputWord,checkWord,i,j +1,0) +1;
        ans = std::min(ans1,ans2);
    }
	return memo.at(i).at(j).at(count) = ans;
}

double ustring::calc_similarity(const std::string_view &inputWord,const std::string_view &checkWord)
{
	static std::vector<std::vector<std::vector<double>>> memo;
	static auto initMemory = true;
	if(initMemory == true)
	{
		initMemory = false;
		memo.resize(100ull);
		for(auto &v : memo)
		{
			v.resize(100ull);
			for(auto &v2 : v)
				v2.resize(100ull,SIM_INF);
		}
	}

	auto r = levenshtein(memo,inputWord,checkWord,0ull,0ull,0ull);
	auto lenInputWord = inputWord.size();
	auto lenCheckWord = checkWord.size();
	auto maxLen = std::max(lenInputWord,lenCheckWord);
	for(auto i=decltype(lenInputWord){0u};i<std::min(lenInputWord,memo.size());++i)
	{
		auto &v0 = memo.at(i);
		for(auto j=decltype(lenCheckWord){0u};j<std::min(lenCheckWord,v0.size());++j)
		{
			auto &v1 = v0.at(j);
			auto l = std::min(maxLen,v1.size());
			std::fill(v1.begin(),v1.begin() +l,SIM_INF);
		}
	}
	return r;
}

void ustring::gather_similar_elements(const std::string_view &baseElement,const std::vector<std::string> &elements,std::vector<size_t> &outElements,uint32_t limit,std::vector<float> *inOutSimilarities)
{
	if(limit > elements.size())
		limit = elements.size();
	std::vector<float> similarities;
	if(inOutSimilarities == nullptr)
		inOutSimilarities = &similarities;
	outElements.reserve(limit +1);
	inOutSimilarities->reserve(limit +1);
	for(auto it=elements.begin();it!=elements.end();++it)
	{
		auto &curEl = *it;
		auto sim = ustring::calc_similarity(baseElement,curEl);
		auto insertAsFirst = true;
		for(auto i=inOutSimilarities->size() -1;i!=static_cast<decltype(inOutSimilarities->size())>(-1);--i)
		{
			auto simOther = inOutSimilarities->at(i);
			if(sim > simOther)
			{
				insertAsFirst = false;
				if(i == inOutSimilarities->size() -1 && inOutSimilarities->size() >= limit)
					break; // This is the most common case, so we bail out early
				inOutSimilarities->insert(inOutSimilarities->begin() +i +1,sim);
				outElements.insert(outElements.begin() +i +1,it -elements.begin());
				break;
			}
		}

		if(insertAsFirst)
		{
			inOutSimilarities->insert(inOutSimilarities->begin(),sim);
			outElements.insert(outElements.begin(),it -elements.begin());
		}

		if(inOutSimilarities->size() > limit)
		{
			inOutSimilarities->erase(inOutSimilarities->end() -1);
			outElements.erase(outElements.end() -1);
		}
	}
}

void ustring::gather_similar_elements(const std::string_view &baseElement,const std::function<std::optional<std::string_view>(void)> &f,std::vector<std::string_view> &outElements,uint32_t limit,std::vector<float> *inOutSimilarities)
{
	std::vector<float> locSimilarities;
	if(inOutSimilarities == nullptr)
		inOutSimilarities = &locSimilarities;
	if(inOutSimilarities->size() != outElements.size())
		throw std::logic_error{"Size of similarities array has to match size of result array!"};
	outElements.reserve(limit +1);
	inOutSimilarities->reserve(limit +1);
	auto curEl = f();
	while(curEl.has_value())
	{
		auto bInserted = false;
		auto percentage = ustring::calc_similarity(baseElement,*curEl);
		for(auto it=outElements.begin();it!=outElements.end();++it)
		{
			auto idx = it -outElements.begin();
			auto &sc = *it;
			if(percentage >= inOutSimilarities->at(idx))
				continue;
			outElements.insert(it,*curEl);
			inOutSimilarities->insert(inOutSimilarities->begin() +idx,percentage);
			if(outElements.size() > limit)
			{
				outElements.pop_back();
				inOutSimilarities->pop_back();
			}
			bInserted = true;
			break;
		}
		if(bInserted == false && outElements.size() < limit)
		{
			outElements.push_back(*curEl);
			inOutSimilarities->push_back(percentage);
		}
		curEl = f();
	}
}
#pragma optimize("",on)
