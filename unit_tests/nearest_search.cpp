#include <iostream>
#include <limits>

typedef float f32;

static f32 f32_abs(const f32 v1, const f32 v2) { f32 d = v2 - v1; return (d < 0) ? -d : d;  }

/*
		{ 2.3f, 4.0f, 5.0f, 6.3f, 7.4f, 7.8f, 8.8f, 9.3f, 10.23f, 34.0f }
					7.9

			rd = dist(arr[len - 1], val)
			ld = dist(arr[0], val)
			dir = (ld < rd) ? 1 : -1;
			i = (dir == 1) ? 0 : (len - 1)
			m_d = (dir == 1) ? ld : rd
			step_size = (len + 1) / 2
			while true:
				i += dir * step_size
				d = dist(arr[i], val)
				if d > m_d:
					dir *= -1
					step_size /= 2
				m_d = d
				if step_size == 0:
					break
			ret i
*/

// searches for a nearest value in the sorted float array
int nearest_search(const f32* arr, int len, f32 val)
{
    if(len == 0)
    	return -1;
    if(val >= arr[len - 1])
    	return len - 1;
    if(val <= arr[0])
    	return 0;
    f32 rd = f32_abs(arr[len - 1], val);
    f32 ld = f32_abs(arr[0], val);
    int dir = (ld < rd) ? 1 : -1;
    int i = (dir == 1) ? 0 : (len - 1);
    f32 p_d = (dir == 1) ? ld : rd;
    f32 m_d = p_d;
    int m_i = i;
    int step_size = (len - 1) / 2;
    while(true)
    {
    	i += dir * step_size;
    	if((i < 0) || (i >= len))
    	{
    		dir *= -1;
    		continue;
    	}
    	f32 d = f32_abs(arr[i], val);
    	if(d > p_d)
    	{
    		dir *= -1;
    		auto old_step_size = step_size;
    		step_size /= 2;
    	}
    	p_d = d;
    	if(m_d > p_d)
    	{
    		m_d = p_d;
    		m_i = i;
    	}
    	if(step_size == 0)
    		break;
    }
    return m_i;
}


int main()
{
	{
		std::cout << "Test Set 1" << std::endl;
		f32 data[12] = { -1.1f, 2.3f, 2.42f, 4.0f, 5.0f, 6.3f, 7.4f, 7.8f, 8.8f, 9.3f, 10.23f, 34.0f };
		f32 inputs[] = { 6.9f, 2.45f, 9.99f, 11.0345f, 33.0f, 9.8f, 9.553f, 1.9f, -0.9f, -1.0f, -2.0f, 44.5f, 2.3f, 4.0, 3.4f, 2.12f, 4.55f, 4.49f, 6.74f, 8.0f, 0.0f, 3.2f, 5.5f, 11.0f, 20.0f, 4.23f };
		f32 expected[] = { 7.4f, 2.42f, 10.23f, 10.23f, 34.0f, 10.23f, 9.3f, 2.3f, -1.1f, -1.1f, -1.1f, 34.0f, 2.3f, 4.0f, 4.0f, 2.3f, 5.0f, 4.0f, 6.3f, 7.8f, -1.1f, 2.42f, 5.0f, 10.23f, 10.23f, 4.0f };
		for(int i = 0; i < (sizeof(inputs) / sizeof(f32)); i++)
		{
			int index = nearest_search(data, 12, inputs[i]);
			if(index != -1)
			{
				if(expected[i] == data[index])
					std::cout << "Passed " << std::endl;
				else
				{
					std::cout << "Failed! Index: " << index << ", Input: " << inputs[i] << ", Output: " << data[index] << ", Expected: " << expected[i] << std::endl;
				}
			}
			else
				std::cout << "Failed! Nearest not found, Input: " << inputs[i] << ", Expected: " << expected[i] << std::endl;
		}
	}

	{
		std::cout << "Test Set 2" << std::endl;
		f32 data[2] = { -1.1f, 2.3f };
		f32 inputs[] = { 6.9f, 2.45f, 9.99f, 11.0345f, 33.0f, 9.8f, 9.553f, 1.9f, -0.9f, -1.0f, -2.0f, 44.5f, 2.3f, 4.0, 3.4f, 2.12f, 4.55f, 4.49f, 6.74f, 8.0f, 0.0f, 3.2f, 5.5f, 11.0f, 20.0f, 4.23f };
		f32 expected[] = { 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, -1.1f, -1.1f, -1.1f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, -1.1f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f };
		for(int i = 0; i < (sizeof(inputs) / sizeof(f32)); i++)
		{
			int index = nearest_search(data, 2, inputs[i]);
			if(index != -1)
			{
				if(expected[i] == data[index])
					std::cout << "Passed " << std::endl;
				else
				{
					std::cout << "Failed! Index: " << index << ", Input: " << inputs[i] << ", Output: " << data[index] << ", Expected: " << expected[i] << std::endl;
				}
			}
			else
				std::cout << "Failed! Nearest not found, Input: " << inputs[i] << ", Expected: " << expected[i] << std::endl;
		}
	}

	{
		std::cout << "Test Set 3" << std::endl;
		f32 data[1] = { -1.1f };
		f32 inputs[] = { 6.9f, 2.45f, 9.99f, 11.0345f, 33.0f, 9.8f, 9.553f, 1.9f, -0.9f, -1.0f, -2.0f, 44.5f, 2.3f, 4.0, 3.4f, 2.12f, 4.55f, 4.49f, 6.74f, 8.0f, 0.0f, 3.2f, 5.5f, 11.0f, 20.0f, 4.23f };
		f32 expected[] = { -1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f,-1.1f };
		for(int i = 0; i < (sizeof(inputs) / sizeof(f32)); i++)
		{
			int index = nearest_search(data, 1, inputs[i]);
			if(index != -1)
			{
				if(expected[i] == data[index])
					std::cout << "Passed " << std::endl;
				else
				{
					std::cout << "Failed! Index: " << index << ", Input: " << inputs[i] << ", Output: " << data[index] << ", Expected: " << expected[i] << std::endl;
				}
			}
			else
				std::cout << "Failed! Nearest not found, Input: " << inputs[i] << ", Expected: " << expected[i] << std::endl;
		}
	}

    return 0;
}
