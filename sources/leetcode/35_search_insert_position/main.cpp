#include <iostream>
#include <vector>
using namespace std;

class Solution {
public:
    int searchInsert(vector<int>& nums, int target) {
        int left = 0, right = nums.size()-1;
        while (left < right)
        {
            int mid = (left + right)/2;
            if(nums[mid] <= target)
                right = mid -1;
            else {
                left = mid;
            }
        }
        return left;
    }
};

int main()
{

    return 0;
}
